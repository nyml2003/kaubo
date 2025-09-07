test_name = input()
integration_dir = "./test/integration/"
import os

# 创建<test.name>文件夹，在文件夹下创建main.py和<test.name>.expected
path = os.getcwd() + "/" + integration_dir
if not os.path.exists(path + test_name):
    os.mkdir(path + test_name)
# 创建文件，不写东西
with open(path + test_name + "/main.py", "w") as f:
    pass
with open(path + test_name + "/" + test_name + ".expected", "w") as f:
    pass
