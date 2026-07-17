"""
QSem-Claude CLI 入口

支持：
  - 单 task 运行 (--task-id)
  - 批量运行 (--batch N)
  - 断点续跑 (--resume)
  - 自定义数据集 (--data)

结果以 JSONL 格式追加写入 results/results.jsonl。
"""
import json
import argparse
import os
from pathlib import Path

from config import *
from agent import QSemAgent

RESULT_FILE = RESULTS_DIR / "results.jsonl"
_current_result_file = RESULT_FILE
TASK_FILES = [DATASET_PATH]


def load_task_by_id(task_id, task_files=None):
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
    if not _current_result_file.exists():
        return set()

    finished = set()
    with open(_current_result_file, "r", encoding="utf-8") as f:
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
    _current_result_file.parent.mkdir(exist_ok=True)
    with open(_current_result_file, "a", encoding="utf-8") as f:
        f.write(json.dumps(result, ensure_ascii=False))
        f.write("\n")


def run_task(agent, task):
    print(f"\n[TASK {task['task_id']}] {task['sut_function']}")

    try:
        result = agent.run(task)
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
    finished = load_finished_tasks() if resume else set()
    tasks = []
    collecting = start_task_id is None
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
        results.append(run_task(agent, task))

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
        tag = ""
        if r.get("illegal_changes"):
            tag = " [DIRTY]"
        err = f" [{r.get('error', '')}]" if r.get("error") else ""
        print(f"  task {r['task_id']}: {status}{tag}{err} ({r['runtime_s']}s)")


def run_checks():
    """验证依赖是否就绪。"""
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

    # 1. Claude CLI
    r = subprocess.run(["claude", "--version"], capture_output=True, text=True)
    check("claude CLI available", r.returncode == 0,
          "Install Claude Code CLI first")

    # 2. QSemOS repo structure
    check(f"QSemOS root '{QSemOS_ROOT}'", QSemOS_ROOT.exists())
    check(f"src/ directory", (QSemOS_ROOT / "src").exists())
    check(f"check_tests/ directory", CHECK_TESTS_DIR.exists())

    # 3. Dataset
    check(f"Dataset '{DATASET_PATH}'", DATASET_PATH.exists())

    # 4. Claude settings
    check(f"Claude settings '{CLAUDE_SETTINGS_SRC}'",
          CLAUDE_SETTINGS_SRC.exists(),
          "Run: claude login")

    # 5. tree-sitter
    try:
        import tree_sitter
        import tree_sitter_c
        check("tree-sitter Python package", True)
    except ImportError:
        check("tree-sitter Python package", False,
              "Run: pip install tree-sitter tree-sitter-c")

    print()
    if all_ok:
        print("All checks passed.")
    else:
        print("Some checks failed. Fix the issues above before running tasks.")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="QSem-Claude Benchmark Runner")
    parser.add_argument("--task-id", type=str, help="要运行的 task ID")
    parser.add_argument("--resume", action="store_true",
                        help="跳过 results.jsonl 中已通过的 task")
    parser.add_argument("--batch", type=int, help="批量运行前 N 个 task")
    parser.add_argument("--start", type=str, default=None,
                        help="从指定 task_id 开始（配合 --batch 使用）")
    parser.add_argument("--data", type=str, nargs="+",
                        help="JSONL 数据集文件路径，默认使用 PrivateAPIEval_agent.jsonl")
    parser.add_argument("--check", action="store_true",
                        help="验证所有依赖是否就绪")
    parser.add_argument("--output", type=str, default=None,
                        help="结果输出文件路径，默认 results/results.jsonl")
    parser.add_argument("--model", type=str, default=None,
                        help="模型名，结果写入 results/<model>/ 和 trajectory/<model>/")
    args = parser.parse_args()

    if args.check:
        run_checks()
        return

    global _current_result_file

    model_traj_dir = None
    if args.model:
        model_results_dir = RESULTS_DIR / args.model
        model_traj_dir = TRAJECTORY_DIR / args.model
        _current_result_file = model_results_dir / "results.jsonl"
    elif args.output:
        _current_result_file = Path(args.output)

    if not args.task_id and not args.batch:
        parser.error("Either --task-id or --batch is required")

    task_files = [Path(f) for f in args.data] if args.data else TASK_FILES

    agent = QSemAgent(QSemOS_ROOT, trajectory_dir=model_traj_dir)

    if args.batch:
        _batch_main(task_files, agent, args.batch,
                    resume=args.resume, start_task_id=args.start)
        return

    if args.resume:
        finished = load_finished_tasks()
        if str(args.task_id) in finished:
            print(f"[SKIP] task {args.task_id} already finished")
            return

    task = load_task_by_id(args.task_id, task_files)
    if task is None:
        print(f"[ERROR] task not found: {args.task_id}")
        return

    run_task(agent, task)


if __name__ == "__main__":
    main()
