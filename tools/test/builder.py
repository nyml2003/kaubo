#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import glob
import os
import sys
import subprocess
import multiprocessing


class Builder:
    """CMake 构建器：封装构建检查、配置、编译逻辑，内部维护路径"""

    def __init__(self, build_type: str = "Release"):
        self.set_build_type(build_type)
        self.build_dir = self._get_abs_build_dir()
        self.cmake_root_dir = self._get_abs_cmake_root()
        self.rebuild = False

    def set_build_type(self, build_type: str) -> None:
        """设置构建类型"""
        self.build_type = self._validate_build_type(build_type)

    def _validate_build_type(self, build_type: str) -> str:
        """验证构建类型，非法时默认 Release"""
        return build_type if build_type in ["Debug", "Release"] else "Release"

    def _get_abs_build_dir(self) -> str:
        """内部计算：构建产物的绝对路径（无需外部传入）"""
        current_script_dir = os.path.dirname(os.path.abspath(__file__))
        return os.path.abspath(
            os.path.join(current_script_dir, "../../engine/build", self.build_type)
        )

    def _get_abs_cmake_root(self) -> str:
        """内部计算：CMake 根目录（engine 目录）的绝对路径"""
        current_script_dir = os.path.dirname(os.path.abspath(__file__))
        return os.path.abspath(os.path.join(current_script_dir, "../../engine"))

    def is_build_ready(self) -> bool:
        """检查构建是否完成（目录存在 + 关键产物存在）"""
        if not os.path.exists(self.build_dir):
            return False
        # 检查关键产物（动态库，适配多系统），避免空目录误判
        lib_patterns = [
            os.path.join(self.build_dir, "lib*.so"),  # Linux
            os.path.join(self.build_dir, "*.dll"),  # Windows
            os.path.join(self.build_dir, "lib*.dylib"),  # macOS
        ]
        for pattern in lib_patterns:
            if glob.glob(pattern):
                return True
        return False

    def build(self) -> None:
        """完整构建流程：创建目录 → CMake 配置 → 并行编译"""
        # 1. 创建构建目录
        print(f"[Builder] 构建类型: {self.build_type}")
        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir, exist_ok=True)
            print(f"[Builder] 创建构建目录: {self.build_dir}")
        elif not self.rebuild:
            print(f"[Builder] 构建目录已存在: {self.build_dir}")
            return

        # 2. CMake 配置
        configure_cmd = [
            "cmake",
            "-G",
            "Ninja",
            f"-DCMAKE_BUILD_TYPE={self.build_type}",
            self.cmake_root_dir,
        ]
        print(f"[Builder] 执行配置: {' '.join(configure_cmd)}")
        try:
            subprocess.run(
                configure_cmd,
                cwd=self.build_dir,
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
            )
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            print(f"[Builder ERROR] 配置失败: {str(e)}")
            sys.exit(1)

        # 3. 并行编译（线程数=CPU核心数）
        build_cmd = ["cmake", "--build", ".", "--", f"-j{multiprocessing.cpu_count()}"]
        print(f"[Builder] 执行编译: {' '.join(build_cmd)}")
        try:
            result = subprocess.run(
                build_cmd,
                cwd=self.build_dir,
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                encoding="utf-8",
            )
            # 如果需要在成功时也显示输出，可以取消下面的注释
            # print(f"[Builder INFO] 编译输出:\n{result.stdout}")
        except subprocess.CalledProcessError as e:
            print(f"[Builder ERROR] 编译失败 (返回码: {e.returncode})")
            if e.stdout:
                print(f"[Builder STDOUT]:\n{e.stdout}")
            if e.stderr:
                print(f"[Builder STDERR]:\n{e.stderr}")
            sys.exit(1)

        print(f"[Builder] {self.build_type} 构建完成！产物路径: {self.build_dir}")

    def parse_args(self):
        """解析命令行参数，确定构建类型和测试类型"""
        args = sys.argv[1:]

        help_msg = (
            "Usage: python test_runner.py [-d | -r]\n"
            "Options:\n"
            "  -d            Debug 模式\n"
            "  -r            Release 模式（默认）\n"
            "  -f            强制重新构建\n"
        )

        if "-h" in args or "--help" in args:
            print(help_msg)
            sys.exit(0)

        for arg in args:
            if arg == "-d":
                self.set_build_type("Debug")
                self.build_dir = self._get_abs_build_dir()
            elif arg == "-r":
                self.set_build_type("Release")
                self.build_dir = self._get_abs_build_dir()
            elif arg == "-f":
                self.rebuild = True
            else:
                print(f"未知参数: {arg}")
                print(help_msg)
                sys.exit(1)


if __name__ == "__main__":
    builder = Builder()
    builder.parse_args()
    builder.build()
