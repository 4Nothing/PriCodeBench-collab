#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "$SCRIPT_DIR"

if [ $# -lt 2 ]; then
    echo "错误：至少需要2个参数，当前传入 $# 个参数" >&2
    echo "直接命令行调试: $(basename "$0") <elf-path> <log-path/bin-path>" >&2
    echo "IDE连接端口调试: $(basename "$0") <elf-path> <log-path/bin-path> <port-num>" >&2
    exit 1
fi

elf_path="$1"
log_path="$2"

# 非bin文件则使用python脚本转换
if [[ "${log_path##*.}" != "bin" ]]; then
    ./coredump_serial_log_parser.py "$log_path" coredump.tmp
    if [ $? -ne 0 ]; then
        echo "错误：coredump_serial_log_parser.py 执行失败" >&2
        exit 1
    fi
    bin_path="coredump.tmp"
else
    bin_path="$log_path"
fi

# IDE连接模式
if [ $# -ge 3 ]; then
    if ! [[ "$3" =~ ^[0-9]+$ ]]; then
        echo "错误：第三个参数必须是数字（端口号）" >&2
        exit 1
    fi
    port_num="$3"
    echo "gdb server start.., wait for connect on port: $port_num"
    ./coredump_gdbserver.py "$elf_path" "$bin_path" --port "$port_num"
else
    echo "gdb server start.."
    ./coredump_gdbserver.py "$elf_path" "$bin_path" &
    set auto-load safe-path /
    gdb-multiarch -x ./.gdbinit "$elf_path"

    if [ -f "coredump.tmp" ]; then
        rm -rf coredump.tmp
    fi
fi
