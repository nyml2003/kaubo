import os

# 定义参数
kaubo_src_dir = r"engine/src"  # 替换为实际路径

# 初始化总行数
total_lines = 0

# 遍历目录中的文件
for root, dirs, files in os.walk(kaubo_src_dir):

    # 遍历当前目录下的文件
    for file_name in files:
        if file_name.endswith((".h", ".cpp")):  # 只处理 .h 和 .cpp 文件
            file_path = os.path.join(root, file_name)

            # 统计文件行数
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                line_count = sum(1 for line in f if line.strip())

            # 输出文件信息
            print(
                f"[INFO] {file_path.replace(kaubo_src_dir, '')}: [{line_count} lines]"
            )

            # 累加总行数
            total_lines += line_count

# 输出总行数
print(f"Total lines of code: {total_lines}")
