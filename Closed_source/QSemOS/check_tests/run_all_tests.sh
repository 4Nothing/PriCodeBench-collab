#!/bin/bash
# 批量测试所有任务（直接编译运行）

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "======================================"
echo "Batch testing all check_tests tasks..."
echo "======================================"

total=0
passed=0
failed=0
failed_tasks=""
LOG_DIR="$SCRIPT_DIR/test_logs"
rm -rf "$LOG_DIR"
mkdir -p "$LOG_DIR"

# 遍历所有任务目录
for task_dir in "$SCRIPT_DIR"/tasks/task_*; do
    [[ -d "$task_dir" ]] || continue
    task_id=$(basename "$task_dir" | sed 's/task_//')
    echo -n "Processing task $task_id... "

    cd "$task_dir"

    # 清理旧文件
    rm -f test test.exe *.o 2>/dev/null || true

    # 编译并运行测试
    make_result=0
    test_result=0
    make > "$LOG_DIR/${task_id}_build.log" 2>&1 || make_result=$?
    if [[ $make_result -eq 0 ]]; then
        ./test > "$LOG_DIR/${task_id}_test.log" 2>&1 || test_result=$?
    fi

    if [[ $make_result -eq 0 && $test_result -eq 0 ]]; then
        echo -e "${GREEN}PASS${NC}"
        ((++passed))
        rm -f "$LOG_DIR/${task_id}_build.log" "$LOG_DIR/${task_id}_test.log"
    else
        echo -e "${RED}FAIL${NC}"
        ((++failed))
        failed_tasks="$failed_tasks $task_id"
        if [[ $make_result -ne 0 ]]; then
            echo "       (build failed, see $LOG_DIR/${task_id}_build.log)"
        else
            echo "       (test failed, see $LOG_DIR/${task_id}_test.log)"
        fi
    fi

    ((++total))
    cd "$SCRIPT_DIR"
done

echo ""
echo "======================================"
echo "Results:"
echo "  Total : $total"
echo "  Passed: $passed"
echo "  Failed: $failed"
echo "======================================"

if [[ -n "$failed_tasks" ]]; then
    echo "Failed tasks:$failed_tasks"
    echo "Error logs saved in: $LOG_DIR"
fi

exit $(( failed > 255 ? 255 : failed ))