# -*- coding: utf-8 -*
import os
from abc import ABC
from typing import Callable, Dict, Optional, List, Tuple

from .native_compiler_bridge import NativeCompilerBridge


class CompilationTask(ABC):
    """
    编译任务抽象基类（整合标准实现的生命周期钩子+日志订阅能力）
    核心流程：绑定编译器桥接器 → 设置配置 → 执行 → 清理
    包含通用默认实现，子类仅需重写需要定制的部分
    """

    def __init__(self):
        self._compiler_bridge: Optional[NativeCompilerBridge] = None  # 编译器桥接器实例
        self._current_config: Dict = {}  # 当前配置
        # 订阅记录：存储（订阅ID, 事件类型），用于后续批量清理
        self._log_subscriptions: List[int] = []
        self._subscriptions: List[Tuple[str, Callable]] = []

    # ------------------------------
    # 1. 编译器桥接器绑定阶段（入口方法+钩子）
    # ------------------------------
    def bind_compiler_bridge(self, compiler_bridge: NativeCompilerBridge) -> None:
        if not isinstance(compiler_bridge, NativeCompilerBridge):
            raise TypeError("桥接器类型不合法")
        self.before_bind_compiler_bridge(compiler_bridge)
        self.on_bind_compiler_bridge(compiler_bridge)
        self.after_bind_compiler_bridge(compiler_bridge)

    def before_bind_compiler_bridge(
        self, compiler_bridge: NativeCompilerBridge
    ) -> None:
        """
        绑定前钩子：默认实现校验桥接器合法性
        子类重写时应调用super()
        """
        # 校验桥接器必须实现核心方法
        required_methods = [
            "subscribe_event",
            "unsubscribe_event",
            "init_with_config",
            "compile",
            "interpret",
        ]
        for method in required_methods:
            if not hasattr(compiler_bridge, method):
                raise AttributeError(f"桥接器缺少必要方法：{method}")

    def on_bind_compiler_bridge(self, compiler_bridge: NativeCompilerBridge) -> None:
        """
        绑定核心钩子：默认实现保存桥接器引用
        子类重写时应调用super()
        """
        self._compiler_bridge = compiler_bridge

    def after_bind_compiler_bridge(self, compiler_bridge: NativeCompilerBridge) -> None:
        """
        绑定后钩子：默认实现订阅程序退出事件
        子类重写时应调用super()
        """

        # def exit_callback(msg: str):
        #     print(f"[系统通知] 程序退出：{msg}")

        # # 订阅EXIT_PROGRAM事件
        # if hasattr(compiler_bridge, "EventType") and hasattr(
        #     compiler_bridge.EventType, "EXIT_PROGRAM"
        # ):
        #     self.subscribe_event("EXIT_PROGRAM", exit_callback)
        #     exit_sub_id = compiler_bridge.subscribe_event("EXIT_PROGRAM", exit_callback)
        #     self._log_subscriptions.append(exit_sub_id)

        for event_type, callback in self._subscriptions:
            self.subscribe_event(event_type, callback)

    # ------------------------------
    # 2. 配置设置阶段（入口方法+钩子）
    # ------------------------------
    def set_config(self, config: Dict) -> None:
        """对外暴露的配置设置入口，触发配置阶段钩子"""
        self.before_set_config(config)
        self.on_set_config(config)
        self.after_set_config(config)

    def before_set_config(self, config: Dict) -> None:
        """
        配置前钩子
        子类重写时应调用super()
        """
        self._current_config = self._compiler_bridge.create_test_config(config)
        if self._current_config.get("file"):
            file_path = self._current_config.get("file")
            if file_path and not os.path.exists(file_path):
                raise FileNotFoundError(f"源码文件不存在：{file_path}")

    def on_set_config(self, config: Dict) -> None:
        """
        配置核心钩子
        子类重写时应调用super()
        """
        self._compiler_bridge.init_with_config(config)

    def after_set_config(self, config: Dict) -> None:
        """
        配置后钩子
        子类重写时应调用super()
        """
        pass

    # ------------------------------
    # 3. 执行阶段（入口方法+钩子）
    # ------------------------------
    def run(self) -> None:
        """对外暴露的执行入口，触发执行阶段钩子"""
        self.before_run()
        self.on_run()
        self.after_run()

    def before_run(self) -> None:
        """
        执行前钩子
        子类重写时应调用super()
        """
        pass

    def on_run(self) -> None:
        """
        执行核心钩子：默认实现根据配置调用编译/解释方法
        子类重写时应调用super()
        """
        try:
            if self._current_config.get("interpret"):
                self._compiler_bridge.interpret()
                return
            if self._current_config.get("compile"):
                self._compiler_bridge.compile()
                return
            if self._current_config.get("interpret_bytecode"):
                self._compiler_bridge.interpret_bytecode()
                return
        finally:
            self._log_subscriptions = []
            self.cleanup()

    def after_run(self) -> None:
        """
        执行后钩子：默认实现提示执行完成
        子类重写时应调用super()
        """
        self._log_subscriptions = []  # 清空订阅记录

    # ------------------------------
    # 4. 清理阶段（入口方法）
    # ------------------------------
    def cleanup(self) -> None:
        """
        资源清理入口：默认实现取消所有日志订阅并重置状态
        子类重写时应调用super().cleanup()
        """
        self.unsubscribe_all_events()

        # 重置状态
        self._compiler_bridge = None
        self._current_config.clear()

    # ------------------------------
    # 对外透出的日志订阅能力（独立接口）
    # ------------------------------
    def subscribe_event(self, event_type: str, callback: Callable[[str], None]) -> int:
        """
        订阅指定类型的日志事件
        :param event_type: 事件类型（如 LOG_INFO/LOG_ERROR，需与桥接器约定）
        :param callback: 事件回调函数（接收日志字符串）
        :return: 订阅ID（用于取消订阅）
        """
        if not hasattr(self._compiler_bridge, "subscribe_event"):
            raise AttributeError("桥接器未实现 subscribe_event 方法")

        sub_id = self._compiler_bridge.subscribe_event(event_type, callback)
        self._log_subscriptions.append(sub_id)  # 记录订阅
        return sub_id

    def unsubscribe_event(self, sub_id: int) -> None:
        """根据订阅ID取消日志订阅"""
        if not hasattr(self._compiler_bridge, "unsubscribe_event"):
            raise AttributeError("桥接器未实现 unsubscribe_event 方法")

        # 查找并取消订阅
        for idx, recorded_sub_id in enumerate(self._log_subscriptions):
            if recorded_sub_id == sub_id:
                self._compiler_bridge.unsubscribe_event(sub_id)
                del self._log_subscriptions[idx]
                return
        raise ValueError(f"无效订阅ID：{sub_id}（未找到记录）")

    def unsubscribe_all_events(self) -> None:
        """取消所有日志订阅（清理阶段推荐调用）"""
        for sub_id in self._log_subscriptions.copy():
            try:
                self.unsubscribe_event(sub_id)
            except Exception as e:
                print(f"取消订阅ID {sub_id} 失败: {str(e)}")

    def subscribe(self, event_type: str, callback: Callable[[str], None]) -> None:
        self._subscriptions.append((event_type, callback))
