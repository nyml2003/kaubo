#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import multiprocessing
from .builder import Builder
from .test_suite import TestSuite


class TestRunner:
    """测试运行器：负责整体流程控制，最少化路径计算"""

    # 常量：仅定义相对路径模板，不计算具体路径
    DEFAULT_BUILD_TYPE = "Release"
    DEFAULT_TEST_TYPES = ["base"]
    BASE_TEST_DIR_REL = "../../engine/test/base"  # 相对路径模板
    ML_TEST_DIR_REL = "../../engine/test/ml_base"  # 相对路径模板
    LIB_DIR_REL_TPL = "../../engine/build/{}"  # lib 路径模板（{} 替换为 build_type）

    def __init__(self):
        # 初始化核心参数（默认值）
        self.build_type = self.DEFAULT_BUILD_TYPE
        self.test_types = self.DEFAULT_TEST_TYPES
        # 仅计算一次：当前脚本所在目录（后续路径基于此目录推导）
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        # 延迟初始化：在 parse_args 后确定的路径（避免重复计算）
        self.builder = None  # Builder 实例
        self.lib_dir = None  # 依赖库绝对路径
        self.test_dirs = {}  # 测试目录字典（key: test_type, value: 绝对路径）
        # 全局结果统计
        self.global_passed = 0
        self.global_total = 0
        self.global_failed = []

        # 解决 Windows 多进程问题
        multiprocessing.freeze_support()

    def parse_args(self) -> None:
        """解析命令行参数，确定构建类型和测试类型"""
        args = sys.argv[1:]
        help_msg = (
            "Usage: python test_runner.py [-d | -r] [-b | -ml | -a]\n"
            "Options:\n"
            "  -d            Debug 模式（构建+测试）\n"
            "  -r            Release 模式（默认，构建+测试）\n"
            "  -b            仅执行 Base 测试\n"
            "  -ml           仅执行 ML 测试\n"
            "  -a            执行 Base + ML 测试\n"
            "  -h/--help     显示帮助信息"
        )

        # 处理帮助参数
        if "-h" in args or "--help" in args:
            print(help_msg)
            sys.exit(0)

        # 解析参数（构建类型 + 测试类型）
        for arg in args:
            if arg == "-d":
                self.build_type = "Debug"
            elif arg == "-r":
                self.build_type = "Release"
            elif arg == "-b":
                self.test_types = ["base"]
            elif arg == "-ml":
                self.test_types = ["ml"]
            elif arg == "-a":
                self.test_types = ["base", "ml"]
            else:
                print(f"[TestRunner ERROR] 未知参数: {arg}")
                print(help_msg)
                sys.exit(1)

        # 仅计算一次：关键路径（基于脚本目录推导绝对路径）
        self._init_paths()

    def _init_paths(self) -> None:
        """初始化所有必要路径（仅调用一次）"""
        # 1. 初始化 Builder（内部已处理构建目录路径）
        self.builder = Builder(build_type=self.build_type)

        # 2. 计算 lib 目录绝对路径（仅一次）
        lib_dir_rel = self.LIB_DIR_REL_TPL.format(self.build_type)
        self.lib_dir = os.path.abspath(os.path.join(self.script_dir, lib_dir_rel))

        # 3. 计算测试目录绝对路径（仅一次）
        self.test_dirs = {
            "base": os.path.abspath(
                os.path.join(self.script_dir, self.BASE_TEST_DIR_REL)
            ),
            "ml": os.path.abspath(os.path.join(self.script_dir, self.ML_TEST_DIR_REL)),
        }

    def _check_and_build(self) -> None:
        """检查构建状态，未构建则自动触发构建"""
        print(f"\n[TestRunner] 检查 {self.build_type} 模式构建产物...")
        self.builder.rebuild = True
        self.builder.build()

    def _run_test_suites(self) -> None:
        """运行所有选定的测试套件"""
        for test_type in self.test_types:
            suite_name = test_type.upper()
            test_dir = self.test_dirs[test_type]

            # 创建并运行测试套件（直接传入预计算的路径）
            test_suite = TestSuite(
                suite_name=suite_name, test_dir=test_dir, lib_dir=self.lib_dir
            )

            # 执行测试并收集结果
            passed, total, failed = test_suite.run()
            self.global_passed += passed
            self.global_total += total
            self.global_failed.extend([f"{suite_name}-{case}" for case in failed])

    def _print_summary(self) -> None:
        """打印全局测试总结"""
        print("\n" + "=" * 60)
        print("[TestRunner 全局总结]")
        print(f"构建模式: {self.build_type}")
        print(f"测试类型: {', '.join(self.test_types)}")
        print(f"总用例数: {self.global_total}")
        print(f"通过用例数: {self.global_passed}")

        if self.global_total == 0:
            print("警告：未执行任何测试用例")
            sys.exit(0)

        pass_rate = (self.global_passed / self.global_total) * 100
        print(f"整体通过率: {pass_rate:.2f}%")

        if self.global_failed:
            print(f"失败用例: {', '.join(self.global_failed)}")
            sys.exit(1)
        else:
            print("所有测试用例通过！")
            sys.exit(0)

    def run(self) -> None:
        """主流程入口：解析参数 → 检查构建 → 运行测试 → 输出总结"""
        self.parse_args()  # 解析参数并初始化路径（仅计算一次路径）
        self._check_and_build()  # 检查构建状态
        self._run_test_suites()  # 运行选定的测试套件
        self._print_summary()  # 输出全局结果


if __name__ == "__main__":
    runner = TestRunner()
    runner.run()
