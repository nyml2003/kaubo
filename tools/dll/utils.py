# -*- coding: utf-8 -*
import sys


class PlatformUtils:
    """平台相关工具类"""

    @staticmethod
    def setup_console_encoding():
        """设置控制台编码为UTF-8（主要针对Windows系统）"""
        if sys.platform == "win32":
            try:
                # 设置控制台输出编码为UTF-8
                sys.stdout.reconfigure(encoding="utf-8")
                sys.stderr.reconfigure(encoding="utf-8")
            except AttributeError:
                # Python 3.6及以下无reconfigure方法，可忽略
                pass
