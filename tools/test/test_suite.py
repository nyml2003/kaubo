import os
import glob
from ..dll.compilation_task_factory import CompilationTaskFactory
from .serializable_callback import SerializableCallback


class TestSuite:
    """测试套件：封装单个套件（Base/ML）的测试执行逻辑，无路径计算"""

    def __init__(self, suite_name: str, test_dir: str, lib_dir: str):
        """
        初始化测试套件
        :param suite_name: 套件名称（如 "BASE", "ML"）
        :param test_dir: 测试用例目录（绝对路径，外部传入）
        :param lib_dir: 依赖库目录（绝对路径，外部传入）
        """
        self.suite_name = suite_name
        self.test_dir = test_dir  # 直接使用外部传入的绝对路径
        self.lib_dir = lib_dir  # 直接使用外部传入的绝对路径
        self.factory = CompilationTaskFactory()  # 编译任务工厂
        self.passed = 0  # 本套件通过用例数
        self.total = 0  # 本套件总用例数
        self.failed_cases = []  # 本套件失败用例名

    def _clean_old_files(self, case_dir: str) -> None:
        """清理测试用例目录的旧文件（.code/.out）"""
        for file in glob.glob(os.path.join(case_dir, "*.code")) + glob.glob(
            os.path.join(case_dir, "*.out")
        ):
            try:
                os.remove(file)
            except Exception as e:
                print(
                    f"[TestSuite {self.suite_name}] 警告：删除旧文件 {file} 失败: {e}"
                )

    @staticmethod
    def compile_callback(msg, case_name: str, suite_name: str):
        print(f"[Compile {suite_name}-{case_name}] {msg}")

    @staticmethod
    def exec_callback(msg, out_file: str):
        with open(out_file, "a", encoding="utf-8") as f:
            f.write(msg + "\n")

    def _compile(self, case_name: str, case_dir: str) -> bool:
        """编译测试用例目录下的源代码文件"""
        files = glob.glob(os.path.join(case_dir, "*.kaubo"))
        if not files:
            print(f"[TestSuite {self.suite_name}] 警告：{case_name} 无 .kaubo 文件")
            return False

        for file in files:
            try:
                # 1. 注册编译配置
                config_id = (
                    f"compile_{self.suite_name}_{case_name}_{os.path.basename(file)}"
                )
                self.factory.register_config(
                    config_id=config_id, config={"compile": True, "file": file}
                )

                # 2. 绑定日志回调（仅打印当前用例的编译日志）

                callback = SerializableCallback(
                    self.compile_callback,
                    case_name=case_name,
                    suite_name=self.suite_name,
                )

                # 3. 执行编译任务
                task_id = f"task_{config_id}"
                self.factory.create_task_in_subprocess(
                    task_id=task_id,
                    config_id=config_id,
                    event_callbacks=[
                        ("LOG_WARNING", callback),
                        ("LOG_ERROR", callback),
                    ],
                    relative_lib_dirs=[self.lib_dir],  # 直接使用外部传入的 lib 路径
                )
                self.factory.manage_task(task_id=task_id, action="join")

            except Exception as e:
                print(
                    f"[TestSuite {self.suite_name}] 错误：{case_name} 编译失败: {str(e)}"
                )
                return False
        return True

    def _execute_bytecode(self, case_name: str, case_dir: str) -> bool:
        """执行编译后的字节码文件，并对比结果"""
        # 1. 选择要执行的 code 文件（优先 main.code，无则选第一个）
        main_code = os.path.join(case_dir, "main.code")
        if os.path.exists(main_code):
            code_file = main_code
        else:
            code_files = glob.glob(os.path.join(case_dir, "*.code"))
            if not code_files:
                print(f"[TestSuite {self.suite_name}] 警告：{case_name} 无 .code 文件")
                return False
            code_file = code_files[0]

        # 2. 准备输出文件和预期结果文件
        out_file = os.path.join(case_dir, f"{case_name}.out")
        expect_file = os.path.join(case_dir, f"{case_name}.expected")
        if not os.path.exists(expect_file):
            print(
                f"[TestSuite {self.suite_name}] 错误：{case_name} 无预期结果文件 {expect_file}"
            )
            return False

        try:
            # 4. 注册执行配置
            config_id = f"exec_{self.suite_name}_{case_name}"
            self.factory.register_config(
                config_id=config_id,
                config={
                    "interpret_bytecode": True,
                    "file": code_file,
                    "show_result": True,
                },
            )

            # 5. 执行字节码任务
            task_id = f"task_{config_id}"
            self.factory.create_task_in_subprocess(
                task_id=task_id,
                config_id=config_id,
                event_callbacks=[
                    (
                        "LOG_INFO",
                        SerializableCallback(self.exec_callback, out_file=out_file),
                    )
                ],
                relative_lib_dirs=[self.lib_dir],  # 直接使用外部传入的 lib 路径
            )
            self.factory.manage_task(task_id=task_id, action="join")

            # 6. 对比输出结果与预期结果
            return self._compare_results(out_file, expect_file)

        except Exception as e:
            print(f"[TestSuite {self.suite_name}] 错误：{case_name} 执行失败: {e.with_traceback()}")
            return False

    def _compare_results(self, out_file: str, expect_file: str) -> bool:
        """对比输出文件与预期结果文件，包括空行差异"""
        # 读取文件内容，保留空行但去除每行首尾空白
        with open(out_file, "r", encoding="utf-8") as f:
            out_lines = [line.strip() for line in f.readlines()]
        with open(expect_file, "r", encoding="utf-8") as f:
            expect_lines = [line.strip() for line in f.readlines()]

        # 对比内容
        if out_lines != expect_lines:
            print(f"[TestSuite {self.suite_name}] 失败：结果不匹配")
            print(f"  实际输出: {out_file}")
            print(f"  预期结果: {expect_file}")

            # 可以添加更详细的差异提示
            print("  差异详情:")
            min_len = min(len(out_lines), len(expect_lines))
            for i in range(min_len):
                if out_lines[i] != expect_lines[i]:
                    print(
                        f"    第{i+1}行: 实际='{out_lines[i]}' 预期='{expect_lines[i]}'"
                    )
            if len(out_lines) > len(expect_lines):
                print(f"    实际结果多了{len(out_lines)-len(expect_lines)}行")
            elif len(out_lines) < len(expect_lines):
                print(f"    实际结果少了{len(expect_lines)-len(out_lines)}行")
            return False
        return True

    def run(self) -> tuple[int, int, list[str]]:
        """执行整个测试套件，返回（通过数，总数，失败用例列表）"""
        # 检查测试目录是否存在
        if not os.path.exists(self.test_dir):
            print(f"[TestSuite {self.suite_name}] 错误：测试目录不存在 {self.test_dir}")
            return 0, 0, []

        # 获取所有测试用例（子目录即单个用例）
        case_dirs = [
            d
            for d in os.listdir(self.test_dir)
            if os.path.isdir(os.path.join(self.test_dir, d))
        ]
        self.total = len(case_dirs)
        print(f"\n[TestSuite {self.suite_name}] 开始执行：共 {self.total} 个用例")

        # 逐个执行测试用例
        for case_name in case_dirs:
            case_dir = os.path.join(self.test_dir, case_name)

            # 步骤1：清理旧文件
            self._clean_old_files(case_dir)
            # 步骤2：编译源代码文件
            if not self._compile(case_name, case_dir):
                self.failed_cases.append(case_name)
                continue
            # 步骤3：执行字节码并对比结果
            if self._execute_bytecode(case_name, case_dir):
                self.passed += 1
                print(f"[TestSuite {self.suite_name}] 用例 {case_name}：PASSED")
            else:
                self.failed_cases.append(case_name)
                print(f"[TestSuite {self.suite_name}] 用例 {case_name}：FAILED")

        # 输出套件执行结果
        pass_rate = (self.passed / self.total * 100) if self.total > 0 else 0.0
        print(f"\n[TestSuite {self.suite_name}] 执行完成：")
        print(f"  总用例数：{self.total}")
        print(f"  通过数：{self.passed}")
        print(f"  通过率：{pass_rate:.2f}%")
        if self.failed_cases:
            print(f"  失败用例：{', '.join(self.failed_cases)}")

        return self.passed, self.total, self.failed_cases
