# -*- coding: utf-8 -*-
import os

# ================= 配置区域 =================
# 输出文件名
OUTPUT_FILE = 'project_code_all.txt'

# 需要扫描的文件后缀
EXTENSIONS = {'.c', '.h'}

# 【黑名单文件夹】这些文件夹里的内容会被完全忽略
# 根据你的截图，'Hardware' 是官方库，'Objects'/'Listings' 是编译垃圾，'FatFs' 是第三方源码
IGNORE_DIRS = {
    'Hardware',  # 官方 HAL 库
    'Objects',  # 编译中间文件
    'Listings',  # 编译列表文件
    'RTE',  # Keil RTE
    'Debug',  # 调试目录
    '.git',  # Git 目录
    '.vscode',  # VSCode 配置
    'FatFs'  # 第三方文件系统 (通常不需要发给AI，除非查文件系统bug)
}

# 【黑名单文件名】如果文件名包含以下字符串，也会被忽略
# 再次加固，防止漏网的官方文件
IGNORE_FILE_PATTERNS = [
    'stm32f1xx_hal',  # 再次确保过滤 HAL 库
    'system_stm32',  # 系统启动文件
    'startup_stm32',  # 汇编启动文件
    'ff.c',  # FatFs 源码
    'ff.h',
    'diskio.c'  # 除非你正在写底层读写接口，否则这个通常也不用看
]


# ================= 脚本逻辑 =================

def is_ignored_file(filename):
    """判断文件名是否在黑名单中"""
    for pattern in IGNORE_FILE_PATTERNS:
        if pattern.lower() in filename.lower():
            return True
    return False


def read_file_content(filepath):
    """尝试使用不同的编码读取文件，解决 Keil 中文注释乱码问题"""
    encodings = ['utf-8', 'gbk', 'gb18030', 'windows-1252']
    for enc in encodings:
        try:
            with open(filepath, 'r', encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    return f"[Error: Unable to decode file with {encodings}]"


def main():
    count = 0
    print(f"正在扫描工程，生成 {OUTPUT_FILE} ...")

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as outfile:
        # 遍历当前目录
        for root, dirs, files in os.walk("."):
            # 1. 修改 dirs 列表，原地剔除黑名单文件夹，阻止 os.walk 进入
            # 这样脚本就不会去扫描 Hardware 文件夹了
            dirs[:] = [d for d in dirs if d not in IGNORE_DIRS and d not in IGNORE_DIRS]

            for file in files:
                # 2. 检查后缀名
                if os.path.splitext(file)[1] not in EXTENSIONS:
                    continue

                # 3. 检查文件名黑名单
                if is_ignored_file(file):
                    continue

                # 4. 拼接路径并读取
                file_path = os.path.join(root, file)
                content = read_file_content(file_path)

                # 5. 写入格式化内容
                outfile.write(f"\n{'=' * 30}\n")
                outfile.write(f"FILE PATH: {file_path}\n")
                outfile.write(f"{'=' * 30}\n")
                outfile.write(content)
                outfile.write("\n\n")

                print(f"已添加: {file_path}")
                count += 1

    print(f"\n成功！共合并了 {count} 个文件。")
    print(f"请将当前目录下的 '{OUTPUT_FILE}' 发送给 AI。")


if __name__ == '__main__':
    main()