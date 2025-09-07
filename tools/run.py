from multiprocessing import freeze_support
import os
import sys

from .test.builder import Builder
from .dll.compilation_task_factory import CompilationTaskFactory


def parse_args():
    """解析命令行参数，确定构建类型和测试类型"""
    args = sys.argv[1:]

    help_msg = (
        "Usage: python test_runner.py [-d | -r]\n"
        "Options:\n"
        "  -d            Debug 模式\n"
        "  -r            Release 模式（默认）\n"
    )

    if "-h" in args or "--help" in args:
        print(help_msg)
        sys.exit(0)

    for arg in args:
        if arg == "-d":
            return "Debug"
        elif arg == "-r":
            return "Release"
        else:
            print(f"未知参数: {arg}")
            print(help_msg)
            sys.exit(1)

    return "Release"  # 默认为 Release 模式


def get_lib_dir():
    LIB_DIR_REL_TPL = "../engine/build/{}"
    script_dir = os.path.dirname(os.path.abspath(__file__))
    build_type = parse_args()
    lib_dir_rel = LIB_DIR_REL_TPL.format(build_type)
    return os.path.abspath(os.path.join(script_dir, lib_dir_rel))


factory = CompilationTaskFactory()
lib_dir = get_lib_dir()


def callback(msg: str):
    print(msg)


if __name__ == "__main__":
    freeze_support()
    builder = Builder()
    builder.parse_args()
    builder.build()
    factory.register_config(
        config_id="temp-run",
        config={
            "file": r"C:\Users\nyml\code\kaubo\engine\test\dev\dev.kaubo",
            "interpret": True,
        },
    )

    subprocess_obj = factory.create_task_in_subprocess(
        task_id="temp-run",
        config_id="temp-run",
        relative_lib_dirs=[lib_dir],
        event_callbacks=[("LOG_INFO", callback)],
    )

