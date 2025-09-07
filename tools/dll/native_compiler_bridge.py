# -*- coding: utf-8 -*
import ctypes
import os
import sys
import json
from typing import Callable, Dict, List


class NativeCompilerBridge:
    def __init__(self, relative_lib_dirs: List[str] = None):
        self.platform = self.detect_platform()
        self.lib_filename = self.get_platform_lib_name()
        self.lib_path = self.get_lib_path(relative_dirs=relative_lib_dirs)
        self.dll = self.load_shared_library()
        self.bind_c_functions()
        self.callbacks: Dict[int, ctypes.CFUNCTYPE] = {}
        self.EventType = type(
            "EventType",
            (),
            {
                "LOG_INFO": 0,
                "LOG_WARNING": 1,
                "LOG_ERROR": 2,
                "LOG_DEBUG": 3,
                "EXIT_PROGRAM": 4,
                "INPUT": 5,
            },
        )

    def detect_platform(self) -> str:
        if sys.platform.startswith("win32"):
            return "windows"
        elif sys.platform.startswith("darwin"):
            return "macos"
        elif sys.platform.startswith("linux"):
            return "linux"
        raise OSError(f"不支持的系统：{sys.platform}（仅支持Windows/macOS/Linux）")

    def get_platform_lib_name(self) -> str:
        lib_base = "kaubo_common"
        if self.platform == "windows":
            return f"{lib_base}.dll"
        elif self.platform == "macos":
            return f"lib{lib_base}.dylib"
        elif self.platform == "linux":
            return f"lib{lib_base}.so"
        raise OSError(f"未知平台：{self.platform}")

    def get_lib_path(self, relative_dirs: List[str] = None) -> str:
        """
        获取动态库路径，依次检查：
        1. 脚本所在目录
        2. 传入的多个相对路径（相对于脚本目录）

        Args:
            relative_dirs: 相对路径列表，如 ["libs", "deps/native"]

        Returns:
            动态库的绝对路径

        Raises:
            FileNotFoundError: 所有路径都未找到动态库时抛出
        """
        # 获取当前脚本所在目录的绝对路径
        script_dir = os.path.dirname(os.path.abspath(sys.argv[0]))

        # 构建要检查的所有路径（先检查脚本目录，再检查相对路径）
        check_paths = [script_dir]  # 首先检查脚本所在目录

        # 添加用户指定的相对路径（相对于脚本目录）
        if relative_dirs:
            for rel_dir in relative_dirs:
                abs_rel_dir = os.path.join(script_dir, rel_dir)
                check_paths.append(abs_rel_dir)

        # 依次检查每个路径
        for check_dir in check_paths:
            lib_path = os.path.join(check_dir, self.lib_filename)
            if os.path.exists(lib_path):
                return lib_path

        # 所有路径都未找到时抛出异常
        error_msg = [f"未找到动态库：{self.lib_filename}"]
        error_msg.append("已检查以下路径：")
        for path in check_paths:
            error_msg.append(f"  - {path}")
        raise FileNotFoundError("\n".join(error_msg))

    def load_shared_library(self) -> ctypes.CDLL:
        try:
            if self.platform == "windows":
                # Windows用WinDLL（兼容C调用约定，处理stdcall）
                return ctypes.WinDLL(self.lib_path)
            else:
                # macOS/Linux用CDLL（标准C调用约定）
                return ctypes.CDLL(self.lib_path)
        except Exception as e:
            raise OSError(
                f"加载动态库失败：{str(e)}\n可能原因：1. 库文件损坏；2. 依赖的其他库缺失（如MSVC运行时）"
            )

    def bind_c_functions(self) -> None:
        """绑定C接口函数，设置参数类型和返回类型（避免类型错误）"""
        self.CEventCallback = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
        self.eventbus_subscribe = self.dll.eventbus_subscribe
        self.eventbus_subscribe.argtypes = (
            ctypes.c_uint32,
            self.CEventCallback,
        )
        self.eventbus_subscribe.restype = ctypes.c_uint32

        self.eventbus_unsubscribe = self.dll.eventbus_unsubscribe
        self.eventbus_unsubscribe.argtypes = (ctypes.c_uint32,)
        self.eventbus_unsubscribe.restype = None

        self.init_config = self.dll.init_config
        self.init_config.argtypes = (ctypes.c_char_p,)  # 参数：JSON字符串（char*）
        self.init_config.restype = None  # 返回：void

        self.compile = self.dll.compile
        self.compile.argtypes = ()
        self.compile.restype = None

        self.interpret = self.dll.interpret
        self.interpret.argtypes = ()
        self.interpret.restype = None

        self.interpret_bytecode = self.dll.interpret_bytecode
        self.interpret_bytecode.argtypes = ()
        self.interpret_bytecode.restype = None

    def subscribe_event(
        self, event_type_name: str, callback: Callable[[str], None]
    ) -> int:
        """
        订阅事件（Python友好接口）
        :param event_type_name: 事件类型名（如"LOG_INFO"、"LOG_ERROR"）
        :param callback: Python回调函数（参数：日志字符串，无返回）
        :return: 订阅ID（用于取消订阅）
        """
        # 验证事件类型
        try:
            event_type = getattr(self.EventType, event_type_name)
        except AttributeError:
            raise ValueError(
                f"无效事件类型：{event_type_name}\n"
                f"可选类型：{[attr for attr in dir(self.EventType) if not attr.startswith('__')]}"
            )

        def c_callback(c_data: ctypes.c_char_p):
            if c_data is not None:
                python_str = c_data.decode(encoding="utf-8", errors="replace")
                callback(python_str)

        c_compatible_callback = self.CEventCallback(c_callback)
        event_id = self.eventbus_subscribe(event_type, c_compatible_callback)
        self.callbacks[event_id] = c_compatible_callback  # 保存引用
        return event_id

    def unsubscribe_event(self, event_id: int) -> None:
        """取消事件订阅（通过订阅ID）"""
        if event_id in self.callbacks:
            self.eventbus_unsubscribe(event_id)
            del self.callbacks[event_id]  # 释放回调引用
        else:
            raise ValueError(f"无效的订阅ID：{event_id}（可能已取消）")

    def init_with_config(self, config: Dict) -> None:
        """
        初始化配置（Python字典转JSON字符串，显式UTF-8编码）
        :param config: 配置字典（与Rust的RunOptions结构对齐）
        """
        try:
            config_json = json.dumps(config, ensure_ascii=False)
            c_config = config_json.encode(encoding="utf-8")
        except Exception as e:
            raise ValueError(f"配置转换失败：{str(e)}（请检查配置字典格式）")
        self.init_config(c_config)

    def create_test_config(self, config: Dict) -> Dict:
        """
        创建测试配置
        """
        if config:
            return {k: v for k, v in config.items() if v is not None and v is not False}
        return {
            "source": "hello world",
            "interpret": True,  # 解释执行模式
        }
