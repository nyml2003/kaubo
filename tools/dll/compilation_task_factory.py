# -*- coding: utf-8 -*
import os
import sys
import json
import multiprocessing
from abc import ABC
import time
from typing import Callable, Dict, Optional, List, Tuple, Type

# 原有模块导入
from .native_compiler_bridge import NativeCompilerBridge
from .compilation_task import CompilationTask
from .utils import PlatformUtils


# ------------------------------
# 顶层子进程任务函数
# ------------------------------
def _subprocess_task_entry(
    task_class: Type[CompilationTask],
    config: Dict,
    callbacks: Optional[List[Tuple[str, Callable[[str], None]]]],
    task_id: str,
    relative_lib_dirs: List[str] = None,
    start_event=None,
    duration_queue: multiprocessing.Queue = None,
) -> None:
    """子进程入口函数"""
    task_instance: CompilationTask = None
    task_start_time = time.perf_counter()
    try:
        PlatformUtils.setup_console_encoding()
        task_instance = task_class()

        # 订阅事件回调
        for event_type, callback in callbacks:
            task_instance.subscribe(event_type, callback)

        # 绑定编译器桥接器
        compiler_bridge = NativeCompilerBridge(relative_lib_dirs)
        task_instance.bind_compiler_bridge(compiler_bridge)

        # 设置配置
        task_instance.set_config(config)

        if start_event:
            start_event.wait()
        # 执行任务
        task_instance.run()

    except Exception as e:
        print(f"[子进程-{task_id}-崩溃] 错误：{str(e)}", file=sys.stderr)
    finally:
        if task_instance:
            task_instance.cleanup()
        task_end_time = time.perf_counter()
        task_duration = round(task_end_time - task_start_time, 2)
        if duration_queue:
            duration_queue.put((task_id, task_duration))


class CompilationTaskFactory:

    def __init__(self):
        self._registered_configs: Dict[str, Dict] = {}  # 配置缓存
        self._task_processes: Dict[str, multiprocessing.Process] = {}  # 进程缓存
        self._task_class: Type[CompilationTask] = CompilationTask  # 任务类
        self._task_events: Dict = {}  # 事件缓存
        self._duration_queue = multiprocessing.Queue()
        self._task_durations: Dict[str, float] = {}

    def register_config(
        self, config_id: str, config: Dict, overwrite: bool = False
    ) -> None:
        """注册配置"""
        if config_id in self._registered_configs and not overwrite:
            raise ValueError(
                f"配置ID [{config_id}] 已存在，若需覆盖请设置 overwrite=True"
            )

        validated_config = self._validate_config(config)
        self._registered_configs[config_id] = validated_config

    def unregister_config(self, config_id: str) -> None:
        """注销配置"""
        if config_id in self._registered_configs:
            del self._registered_configs[config_id]
            print(f"配置 [{config_id}] 注销成功")
        else:
            raise KeyError(f"配置ID [{config_id}] 未注册")

    def set_task_class(self, task_class: Type[CompilationTask]) -> None:
        """设置自定义任务类"""
        if not issubclass(task_class, CompilationTask):
            raise TypeError("自定义任务类必须继承自 CompilationTask")
        self._task_class = task_class
        print(f"任务类已设置为：{task_class.__name__}")

    def create_task_in_subprocess(
        self,
        task_id: str,
        config_id: str,
        event_callbacks: Optional[List[Tuple[str, Callable[[str], None]]]] = None,
        relative_lib_dirs: List[str] = None,
        enable_start_event: bool = False,
    ) -> multiprocessing.Process:
        """创建子进程执行任务"""
        # 校验输入
        if config_id not in self._registered_configs:
            raise KeyError(f"配置ID [{config_id}] 未注册，请先调用 register_config")
        if task_id in self._task_processes:
            raise ValueError(f"任务ID [{task_id}] 已存在，请勿重复创建")

        # 获取配置
        target_config = self._registered_configs[config_id]

        if event_callbacks is None:
            event_callbacks = []
        start_event = multiprocessing.Event() if enable_start_event else None
        # 创建子进程
        process = multiprocessing.Process(
            target=_subprocess_task_entry,
            args=(
                self._task_class,  # 任务类
                target_config,  # 配置字典
                event_callbacks,  # 事件回调
                task_id,  # 任务ID
                relative_lib_dirs,
                start_event,
                self._duration_queue,
            ),
            name=f"CompilationTask-{task_id}",
        )

        # 启动进程
        process.start()
        self._task_processes[task_id] = process
        self._task_events[task_id] = start_event
        return process

    def manage_task(
        self, task_id: str, action: str = "join", timeout: Optional[float] = None
    ) -> None:
        """管理子进程任务"""
        if task_id not in self._task_processes:
            raise KeyError(f"任务ID [{task_id}] 不存在")

        process = self._task_processes[task_id]
        if action == "join":
            process.join(timeout)
            if process.is_alive():
                print(f"任务 [{task_id}] 等待超时（{timeout}秒），进程仍在运行")
            else:
                del self._task_processes[task_id]
        elif action == "terminate":
            if process.is_alive():
                process.terminate()
                process.join(5)
                print(f"任务 [{task_id}] 已强制终止")
            else:
                print(f"任务 [{task_id}] 已结束，无需终止")
            del self._task_processes[task_id]
        else:
            raise ValueError(f"不支持的操作类型：{action}，仅支持 join/terminate")

    def _validate_config(self, config: Dict) -> Dict:
        """配置校验"""
        if not isinstance(config, Dict):
            raise TypeError("配置必须是字典类型")

        validated = {
            k: v for k, v in config.items() if v is not None and v is not False
        }

        if not validated:
            validated = {
                "source": "print('Hello from factory-managed task!')",
                "interpret": True,
            }

        if "file" in validated:
            file_path = os.path.abspath(validated["file"])
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"源码文件不存在：{file_path}")
            validated["file"] = file_path

        return validated

    def fetch_task_durations(self) -> None:
        """从队列中获取子进程传回的时长数据并存储"""
        while not self._duration_queue.empty():
            task_id, duration = self._duration_queue.get()
            self._task_durations[task_id] = duration

    def get_task_duration(self, task_id: Optional[str] = None) -> Optional[float]:
        """获取任务时长（需先调用fetch_task_durations更新数据）"""
        self.fetch_task_durations()  # 先同步最新数据
        if task_id is None:
            # 聚合所有任务的时长
            return sum(self._task_durations.values())
        return self._task_durations.get(task_id)
