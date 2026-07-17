"""Analyze benchmark results and update stats."""
import json
from collections import Counter, defaultdict
from pathlib import Path

RESULTS_DIR = Path(__file__).resolve().parent / "results"
TRAJECTORY_DIR = Path(__file__).resolve().parent / "trajectory"

# Original results
orig = []
with open(RESULTS_DIR / "results_deepseek-v4-pro.jsonl") as f:
    for line in f:
        if line.strip():
            orig.append(json.loads(line))

total = len(orig)
passed = sum(1 for r in orig if r["passed"])
failed = total - passed
completed = sum(1 for r in orig if r.get("claude_completed"))

# Error categories
cats = Counter()
failed_tasks = []
for r in orig:
    if not r["passed"]:
        cat = r.get("error_category", "unknown")
        cats[cat] += 1
        failed_tasks.append(r)

# Re-run results
rerun = []
rerun_file = RESULTS_DIR / "results.jsonl"
if rerun_file.exists():
    with open(rerun_file) as f:
        for line in f:
            if line.strip():
                rerun.append(json.loads(line))

rerun_map = {}
for rr in rerun:
    tid = str(rr["task_id"])
    if tid not in rerun_map or rr["passed"]:
        rerun_map[tid] = rr

# Runtime stats
times = [r["runtime_s"] for r in orig]
times.sort()

# By directory
dir_passed = defaultdict(lambda: [0, 0])
for r in orig:
    d = r["snapshot"]["task"]["source_path"].split("/")[0]
    dir_passed[d][1] += 1
    if r["passed"]:
        dir_passed[d][0] += 1

# By file type
type_passed = defaultdict(lambda: [0, 0])
for r in orig:
    ext = r["snapshot"]["task"]["source_path"].rsplit(".", 1)[-1]
    type_passed[ext][1] += 1
    if r["passed"]:
        type_passed[ext][0] += 1

# --- Analyse trajectory logs for remaining failures ---
# Read trajectory logs for all failed tasks
trajectory_dir = TRAJECTORY_DIR / "results_deepseek-v4-pro"
failure_details = []

for r in orig:
    tid = str(r["task_id"])
    # Skip if fixed by rerun
    if tid in rerun_map and rerun_map[tid]["passed"]:
        continue
    if r["passed"]:
        continue

    task = r["snapshot"]["task"]
    log_file = trajectory_dir / f"task_{tid}.log"

    # Read log
    log_content = ""
    if log_file.exists():
        try:
            with open(log_file, encoding="utf-8", errors="replace") as f:
                log_content = f.read()
        except Exception:
            pass

    # Extract key info from log
    error_cat = r.get("error_category", "unknown")
    error = r.get("error", "")

    # Find stderr output from harness verify section
    stderr_info = ""
    if "--- stderr ---" in log_content:
        parts = log_content.split("--- stderr ---", 1)
        if len(parts) > 1:
            stderr_lines = parts[1].strip().split("\n")[:15]
            stderr_info = "\n".join(stderr_lines)

    detail = {
        "task_id": tid,
        "function": task["sut_function"],
        "source_path": task["source_path"],
        "error_category": error_cat,
        "error": error,
        "runtime": r["runtime_s"],
        "illegal": r.get("illegal_changes", []),
        "stderr_preview": stderr_info[:500] if stderr_info else "",
    }
    failure_details.append(detail)


# ========== Print ==========
print("=" * 70)
print("=== ORIGINAL RESULTS ===")
print(f"Total: {total}")
print(f"Passed: {passed} ({passed/total*100:.1f}%)")
print(f"Failed: {failed} ({failed/total*100:.1f}%)")
print(f"Claude completed: {completed}")

# Clean/Dirty breakdown
clean_pass = sum(1 for r in orig if r["passed"] and not r.get("illegal_changes"))
dirty_pass = sum(1 for r in orig if r["passed"] and r.get("illegal_changes"))
dirty_fail = sum(1 for r in orig if not r["passed"] and r.get("illegal_changes"))
dirty_total = dirty_pass + dirty_fail

print(f"\n=== CORE METRICS ===")
print(f"  Test pass rate:        {passed}/{total} ({passed/total*100:.1f}%)" if total else "")
print(f"  Clean pass:            {clean_pass}/{total} ({clean_pass/total*100:.1f}%)")
print(f"    (integrity OK, tests passed)")
print(f"  Dirty pass:            {dirty_pass}/{total} ({dirty_pass/total*100:.1f}%)")
print(f"    (integrity violated, tests still passed)")
print(f"  Dirty fail:            {dirty_fail}/{total} ({dirty_fail/total*100:.1f}%)")
print(f"    (integrity violated, tests failed)")
print(f"  Integrity OK rate:     {total - dirty_total}/{total} ({(total - dirty_total)/total*100:.1f}%)")

if dirty_pass:
    print(f"\n=== DIRTY PASS ({dirty_pass} tasks) ===")
    for r in orig:
        if r["passed"] and r.get("illegal_changes"):
            v = "; ".join(r["illegal_changes"])
            print(f"  task {r['task_id']}: {v[:120]}")

print(f"\n=== ERROR CATEGORIES ===")
for cat, cnt in cats.most_common():
    print(f"  {cat}: {cnt} ({cnt/failed*100:.1f}%)")

print(f"\n=== RUNTIME ===")
n = len(times)
print(f"Min: {times[0]:.0f}s")
print(f"P25: {times[n//4]:.0f}s")
print(f"Median: {times[n//2]:.0f}s")
print(f"P75: {times[n*3//4]:.0f}s")
print(f"P90: {times[n*9//10]:.0f}s")
print(f"Max: {times[-1]:.0f}s")
print(f"Total: {sum(times)/3600:.1f}h")

print(f"\n=== BY DIRECTORY ===")
for d in sorted(dir_passed):
    p, t = dir_passed[d]
    print(f"  {d}: {p}/{t} ({p/t*100:.1f}%)")

print(f"\n=== BY FILE TYPE ===")
for ext in sorted(type_passed):
    p, t = type_passed[ext]
    print(f"  .{ext}: {p}/{t} ({p/t*100:.1f}%)")

print(f"\n=== RE-RUN SUMMARY ===")
rerun_passed = sum(1 for tid, r in rerun_map.items() if r["passed"])
rerun_failed = len(rerun_map) - rerun_passed
print(f"Re-run unique tasks: {len(rerun_map)}, passed: {rerun_passed}, failed: {rerun_failed}")

print(f"\n=== REMAINING FAILURES AFTER RE-RUNS ===")
remaining = [
    r for r in orig
    if not r["passed"]
    and not (str(r["task_id"]) in rerun_map and rerun_map[str(r["task_id"])]["passed"])
]
rcats = Counter(r.get("error_category", "unknown") for r in remaining)
for cat, cnt in rcats.most_common():
    print(f"  {cat}: {cnt}")

print(f"\n=== FAILURE DETAILS BY CATEGORY ===")
for cat in ["compile_error", "test_failure", "test_not_executed", "crash", "illegal_modifications"]:
    details = [d for d in failure_details if d["error_category"] == cat]
    if not details:
        continue
    print(f"\n--- {cat} ({len(details)} tasks) ---")
    for d in details:
        func = d["function"]
        src = d["source_path"]
        err = d.get("error", "")
        illegal = "; ".join(d.get("illegal", [])) if d.get("illegal") else ""
        stderr = d.get("stderr_preview", "")
        print(f"\n  Task {d['task_id']}: {func} ({src}) runtime={d['runtime']:.0f}s")
        if illegal:
            print(f"    ILLEGAL: {illegal[:200]}")
        if err:
            print(f"    ERROR: {err[:200]}")
        if stderr:
            for line in stderr.split("\n")[:5]:
                print(f"    STDERR: {line[:200]}")
