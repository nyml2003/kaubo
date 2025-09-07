import pickle
from typing import Callable, Any


class SerializableCallback:
    """
    通用可序列化回调类，用于包装需要跨进程传递的带参数函数

    解决Python多进程中局部函数、带参数函数无法被pickle序列化的问题
    """

    def __init__(self, func: Callable, *args, **kwargs):
        """
        初始化回调包装器

        :param func: 要包装的函数（需在模块顶层定义）
        :param args: 函数的位置参数
        :param kwargs: 函数的关键字参数
        """
        self.func = func
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *call_args, **call_kwargs) -> Any:
        """
        调用包装的函数，合并初始化时的参数和调用时的参数

        :param call_args: 调用时的位置参数
        :param call_kwargs: 调用时的关键字参数
        :return: 被包装函数的返回值
        """
        # 合并参数：初始化时的参数 + 调用时的参数
        all_args = self.args + call_args
        all_kwargs = {**self.kwargs, **call_kwargs}

        # 调用原始函数
        return self.func(*all_args, **all_kwargs)

    def __reduce__(self):
        """
        自定义pickle序列化逻辑，确保对象能被正确序列化

        :return: 序列化所需的元组
        """
        return (
            SerializableCallback,
            (self.func,),
            {"args": self.args, "kwargs": self.kwargs},
        )


# ------------------------------
# 使用示例
# ------------------------------
if __name__ == "__main__":
    # 1. 定义一个普通函数（需在模块顶层）
    def log_callback(msg, case_name: str, suite_name: str):
        print(f"[Compile {suite_name}-{case_name}] {msg}")

    # 2. 创建可序列化的回调实例，绑定参数
    callback = SerializableCallback(
        log_callback, case_name="test_case_1", suite_name="base_suite"
    )

    # 3. 测试调用
    callback("编译开始...")  # 输出: [Compile base_suite-test_case_1] 编译开始...

    # 4. 测试序列化/反序列化
    try:
        # 序列化
        pickled = pickle.dumps(callback)
        # 反序列化
        unpickled = pickle.loads(pickled)
        # 验证反序列化后的功能
        unpickled("编译完成!")  # 输出: [Compile base_suite-test_case_1] 编译完成!
        print("序列化测试成功")
    except Exception as e:
        print(f"序列化测试失败: {e}")
