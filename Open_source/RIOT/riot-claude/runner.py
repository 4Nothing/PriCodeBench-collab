"""
RIOT-Claude CLI 入口

提供命令行接口，支持：
  - 单 task 运行 (--task-id)
  - 批量运行 (--batch N)
  - 断点续跑 (--resume)
  - 自定义数据集 (--data)

结果以 JSONL 格式追加写入 results/results.jsonl。
"""
import json
import argparse
from pathlib import Path

from config import *
from agent import ClaudeAgent


RESULT_FILE = RESULTS_DIR / "results.jsonl"

# 默认数据集文件（从 config 读取，可通过 --data CLI 参数覆盖）
TASK_FILES = [Path(f) for f in DEFAULT_TASK_FILES]


def load_task_by_id(task_id, task_files=None):
    """从指定的 JSONL 文件中搜索 task_id。

    遍历每个文件的每一行，解析 JSON，匹配 task_id 字段。
    返回第一个匹配的 task 对象，或 None（未找到）。

    Args:
        task_id:  要查找的 task ID（字符串或数字均可）
        task_files: JSONL 文件路径列表，默认使用 TASK_FILES
    Returns:
        匹配的 task dict，或 None
    """
    if task_files is None:
        task_files = TASK_FILES

    for path in task_files:
        with open(path, "r", encoding="utf-8") as f:
            for line in f:
                if not line.strip():
                    continue
                try:
                    obj = json.loads(line)
                except json.JSONDecodeError:
                    continue
                if str(obj["task_id"]) == str(task_id):
                    return obj

    return None


def load_finished_tasks():
    """读取 results.jsonl，返回所有已通过的 task_id 集合。

    用于 --resume 模式：跳过已经成功完成的 task。
    """
    if not RESULT_FILE.exists():
        return set()

    finished = set()
    with open(RESULT_FILE, "r", encoding="utf-8") as f:
        for line in f:
            if not line.strip():
                continue
            try:
                obj = json.loads(line)
            except json.JSONDecodeError:
                continue
            if obj.get("passed"):
                finished.add(str(obj["task_id"]))

    return finished


def append_result(result):
    """将 task 结果以 JSON 格式追加写入 results.jsonl。

    每行一个 JSON 对象，方便后续分析和统计。
    """
    RESULT_FILE.parent.mkdir(exist_ok=True)

    with open(RESULT_FILE, "a", encoding="utf-8") as f:
        f.write(json.dumps(result, ensure_ascii=False))
        f.write("\n")


def run_task(agent, task, dataset_paths=None):
    """执行单个 task 并打印结果。

    Args:
        agent: ClaudeAgent 实例
        task:  task dict（含 source_path, sut_function, run_command 等字段）
        dataset_paths: 可选，数据集 JSONL 文件路径列表
    Returns:
        result dict（from agent.run()）
    """
    print(f"\n[TASK {task['task_id']}] {task['sut_function']}")

    try:
        result = agent.run(task, dataset_paths=dataset_paths)
    except Exception as e:
        result = {
            "task_id": task["task_id"],
            "passed": False,
            "runtime_s": -1,
            "illegal_changes": [],
            "error": str(e),
            "error_category": "exception",
        }
        print(f"[ERROR] {e}")

    append_result(result)
    print("PASS" if result["passed"] else "FAIL")
    return result


def _batch_main(task_files, agent, n, resume=False, start_task_id=None):
    """批量模式：加载 task 并执行。resume 为 True 时跳过已通过的 task。
    start_task_id 指定从哪个 task_id 开始（包含该 task），直观定位。"""
    finished = load_finished_tasks() if resume else set()
    tasks = []
    collecting = start_task_id is None  # 没指定 start 则从头开始收集
    start_num = int(start_task_id) if start_task_id else 0
    for path in task_files:
        with open(path, "r", encoding="utf-8") as f:
            for line in f:
                if not line.strip():
                    continue
                try:
                    obj = json.loads(line)
                except json.JSONDecodeError:
                    continue
                tid = str(obj["task_id"])
                if resume and tid in finished:
                    print(f"  [SKIP] task {tid} already finished")
                    continue
                if not collecting:
                    if int(tid) >= start_num:
                        collecting = True
                    else:
                        continue
                tasks.append(obj)
                if len(tasks) >= n:
                    break
        if len(tasks) >= n:
            break

    print(f"Batch: {len(tasks)} tasks{', resume mode' if resume else ''}")
    print()

    results = []
    for task in tasks:
        results.append(run_task(agent, task, dataset_paths=task_files))

    # 打印批量汇总
    print(f"\n{'='*50}")
    print(f"SUMMARY ({len(results)} tasks)")
    print(f"{'='*50}")
    total = len(results)
    passed = sum(1 for r in results if r["passed"])
    failed = total - passed
    clean_pass = sum(1 for r in results if r["passed"] and not r.get("illegal_changes"))
    dirty_pass = sum(1 for r in results if r["passed"] and r.get("illegal_changes"))
    dirty_fail = sum(1 for r in results if not r["passed"] and r.get("illegal_changes"))

    print(f"  Test pass rate:  {passed}/{total} ({passed/total*100:.1f}%)" if total else "")
    print(f"  Clean pass:      {clean_pass}/{total} ({clean_pass/total*100:.1f}%)" if total else "")
    print(f"  Dirty pass:      {dirty_pass}/{total} ({dirty_pass/total*100:.1f}%)" if total else "")
    print(f"  Dirty fail:      {dirty_fail}/{total} ({dirty_fail/total*100:.1f}%)" if total else "")
    print(f"  ---")
    print(f"  Integrity OK rate: {total - dirty_pass - dirty_fail}/{total} ({(total - dirty_pass - dirty_fail)/total*100:.1f}%)" if total else "")

    for r in results:
        status = "PASS" if r["passed"] else "FAIL"
        tag = " [DIRTY]" if r.get("illegal_changes") else ""
        err = f" [{r.get('error', '')}]" if r.get("error") else ""
        print(f"  task {r['task_id']}: {status}{tag}{err} ({r['runtime_s']}s)")


def run_checks():
    """验证所有外部依赖是否就绪，发现问题打印错误并返回 False。

    检查项：
      1. Docker 是否运行
      2. Docker 镜像是否存在
      3. tree-sitter grammar 文件是否存在
      4. Claude settings.json 是否存在
      5. RIOT 仓库结构是否正常
      6. 数据集文件是否存在
    """
    import subprocess
    import sys

    all_ok = True

    def check(label, ok, detail=""):
        nonlocal all_ok
        status = "OK" if ok else "FAIL"
        msg = f"  [{status}] {label}"
        if detail and not ok:
            msg += f"  — {detail}"
        print(msg)
        if not ok:
            all_ok = False

    print("=== Dependency Check ===\n")

    # 1. Docker
    r = subprocess.run(["docker", "info"], capture_output=True, text=True)
    check("Docker daemon running", r.returncode == 0,
          "Is Docker Desktop started?")

    # 2. Docker image
    r = subprocess.run(
        ["docker", "image", "inspect", DOCKER_IMAGE],
        capture_output=True, text=True,
    )
    check(f"Docker image '{DOCKER_IMAGE}'", r.returncode == 0,
          f"Run: docker build -t {DOCKER_IMAGE} .")

    # 3. tree-sitter grammar
    ts_lib = BENCH_ROOT / "build" / "my-languages.so"
    check(f"tree-sitter grammar '{ts_lib}'", ts_lib.exists(),
          f"Expected at {ts_lib}")

    # 4. Claude settings
    check(f"Claude settings '{CLAUDE_SETTINGS_SRC}'",
          CLAUDE_SETTINGS_SRC.exists(),
          "Run: claude login (on Windows host)")

    # 5. RIOT repo structure
    expected_dirs = ["core", "sys", "cpu", "drivers", "Makefile"]
    missing = [d for d in expected_dirs
               if not (RIOT_ROOT / d).exists()]
    check("RIOT repo structure", len(missing) == 0,
          f"Missing: {', '.join(missing)}")

    # 6. Dataset files
    for f in DEFAULT_TASK_FILES:
        exists = Path(f).exists()
        check(f"Dataset '{f}'", exists)

    print()
    if all_ok:
        print("All checks passed.")
    else:
        print("Some checks failed. Fix the issues above before running tasks.")
        sys.exit(1)


def main():
    """CLI 主入口。

    用法:
      python3 runner.py --task-id 1
      python3 runner.py --batch 10
      python3 runner.py --task-id 5 --resume
      python3 runner.py --batch 20 --data my_tasks.jsonl
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("--task-id", type=str, help="要运行的 task ID")
    parser.add_argument("--resume",
        action="store_true",
        help="跳过 results.jsonl 中已通过的 task")
    parser.add_argument("--batch", type=int, help="批量运行前 N 个 task")
    parser.add_argument("--start", type=str, default=None,
        help="从指定 task_id 开始（配合 --batch 使用）")
    parser.add_argument("--data", type=str, nargs="+",
        help="JSONL 数据集文件路径（可指定多个），默认使用 riot_tasks_with_commands.c.jsonl + .h.jsonl")
    parser.add_argument("--check", action="store_true",
        help="验证所有外部依赖是否就绪（Docker, image, grammar, settings, datasets）")
    args = parser.parse_args()

    if args.check:
        run_checks()
        return

    if not args.task_id and not args.batch:
        parser.error("Either --task-id or --batch is required")

    # 确定数据集文件
    task_files = [Path(f) for f in args.data] if args.data else TASK_FILES

    agent = ClaudeAgent(RIOT_ROOT)

    if args.batch:
        _batch_main(task_files, agent, args.batch, resume=args.resume, start_task_id=args.start)
        return

    # --resume 检查：该 task 是否已成功完成
    if args.resume:
        finished = load_finished_tasks()
        if str(args.task_id) in finished:
            print(f"[SKIP] task {args.task_id} already finished")
            return

    # 单个 task
    task = load_task_by_id(args.task_id, task_files)
    if task is None:
        print(f"[ERROR] task not found: {args.task_id}")
        return

    run_task(agent, task, dataset_paths=task_files)


if __name__ == "__main__":
    main()
