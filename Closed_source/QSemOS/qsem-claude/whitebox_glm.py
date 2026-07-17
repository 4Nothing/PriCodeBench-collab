"""
GLM-5.1 White-Box Failure Analysis
==================================
对 QSemOS GLM-5.1 benchmark 的 46 个失败任务做三级分类。
"""
import json
from pathlib import Path
from collections import Counter

RESULTS_DIR = Path(__file__).resolve().parent / "results"
TRAJECTORY_DIR = Path(__file__).resolve().parent / "trajectory"

L1_LABELS = {
    "compile_error": "编译失败",
    "crash": "运行崩溃",
    "test_failure": "测试失败",
    "test_not_executed": "测试未执行",
    "illegal_modifications": "完整性违规",
}


def load_results(model="glm-5.1"):
    results = []
    with open(RESULTS_DIR / model / "results.jsonl", encoding='utf-8') as f:
        for line in f:
            results.append(json.loads(line))
    return results


def get_module(source_path):
    """从 source_path 提取模块名"""
    parts = source_path.replace("src/", "").split("/")
    if len(parts) >= 3:
        return f"{parts[0]}/{parts[1]}"
    elif len(parts) >= 2:
        return f"{parts[0]}/{parts[1]}"
    return source_path


def analyze_glm():
    results = load_results("glm-5.1")
    failed = [r for r in results if not r["passed"]]
    passed = sum(1 for r in results if r["passed"])
    total = len(results)

    print("=" * 70)
    print("GLM-5.1 White-Box Failure Analysis")
    print(f"Total: {total} | Pass: {passed} | Failed: {len(failed)}")
    print(f"Pass rate: {passed/total*100:.1f}%")
    print("=" * 70)

    # ---- L1 分布 ----
    l1 = Counter(r["error_category"] for r in failed)
    print("\n## L1 失败大类")
    for cat, n in l1.most_common():
        print(f"  {L1_LABELS.get(cat, cat)}: {n} ({n/len(failed)*100:.1f}%)")

    # ---- 模块通过率 ----
    print("\n## 模块通过率")
    module_stats = {}
    for r in results:
        mod = get_module(r["snapshot"]["task"]["source_path"])
        if mod not in module_stats:
            module_stats[mod] = {"total": 0, "passed": 0}
        module_stats[mod]["total"] += 1
        if r["passed"]:
            module_stats[mod]["passed"] += 1

    for mod in sorted(module_stats.keys()):
        s = module_stats[mod]
        rate = s["passed"] / s["total"] * 100
        print(f"  {mod:<28} {s['passed']:>3}/{s['total']:<3} {rate:5.1f}%")

    # ---- 失败任务详情 ----
    print(f"\n## 46 个失败任务详情")

    # compile_error
    compile_tasks = [r for r in failed if r["error_category"] == "compile_error"]
    print(f"\n### 编译失败 ({len(compile_tasks)} 例)")
    for r in compile_tasks:
        func = r["snapshot"]["task"]["sut_function"]
        mod = get_module(r["snapshot"]["task"]["source_path"])
        print(f"  task {r['task_id']:<3} [{func}] ({mod})")

    # test_failure
    test_fail_tasks = [r for r in failed if r["error_category"] == "test_failure"]
    print(f"\n### 测试失败 ({len(test_fail_tasks)} 例)")
    for r in test_fail_tasks:
        func = r["snapshot"]["task"]["sut_function"]
        mod = get_module(r["snapshot"]["task"]["source_path"])
        print(f"  task {r['task_id']:<3} [{func}] ({mod})")

    # ---- 与 DS-v4-pro 对比 ----
    ds_results = load_results("deepseek-v4-pro")
    ds_by_id = {r["task_id"]: r for r in ds_results}
    glm_by_id = {r["task_id"]: r for r in results}

    print("\n## 与 DS-v4-pro 交汇")
    both_pass = 0
    both_fail = 0
    ds_only_pass = 0
    glm_only_pass = 0
    ds_fail_ids = []
    glm_fail_ids = []

    for tid in sorted(glm_by_id.keys()):
        gr = glm_by_id[tid]
        dr = ds_by_id.get(tid)
        if not dr:
            continue
        if gr["passed"] and dr["passed"]:
            both_pass += 1
        elif not gr["passed"] and not dr["passed"]:
            both_fail += 1
            func = gr["snapshot"]["task"]["sut_function"]
            print(f"  task {tid:<3} [{func}]: BOTH FAIL — GLM={gr['error_category']} DS={dr['error_category']}")
        elif gr["passed"] and not dr["passed"]:
            glm_only_pass += 1
            ds_fail_ids.append(tid)
        elif not gr["passed"] and dr["passed"]:
            ds_only_pass += 1
            glm_fail_ids.append(tid)

    print(f"\n  Both pass: {both_pass}")
    print(f"  Both fail: {both_fail}")
    print(f"  GLM pass, DS fail: {glm_only_pass}")
    print(f"  GLM fail, DS pass: {ds_only_pass}")

    if ds_fail_ids:
        print(f"\n  DS-v4-pro 独有失败 (GLM 通过):")
        for tid in ds_fail_ids:
            dr = ds_by_id[tid]
            func = dr["snapshot"]["task"]["sut_function"]
            print(f"    task {tid}: [{func}] DS={dr['error_category']}")

    if glm_fail_ids:
        print(f"\n  GLM-5.1 独有失败 (DS 通过):")
        for tid in glm_fail_ids:
            gr = glm_by_id[tid]
            func = gr["snapshot"]["task"]["sut_function"]
            print(f"    task {tid}: [{func}] GLM={gr['error_category']}")

    # ---- 时间对比 ----
    print("\n## 时间统计")
    times = sorted([r["runtime_s"] for r in results])
    ds_times = sorted([r["runtime_s"] for r in ds_results])
    print(f"  GLM-5.1:   total={sum(times)/3600:.1f}h, P50={times[len(times)//2]:.0f}s, P90={times[int(len(times)*0.9)]:.0f}s, max={times[-1]:.0f}s")
    print(f"  DS-v4-pro: total={sum(ds_times)/3600:.1f}h, P50={ds_times[len(ds_times)//2]:.0f}s, P90={ds_times[int(len(ds_times)*0.9)]:.0f}s, max={ds_times[-1]:.0f}s")

    # ---- 完整性 ----
    glm_dirty = [r for r in results if r.get("illegal_changes")]
    ds_dirty = [r for r in ds_results if r.get("illegal_changes")]
    print(f"\n  GLM-5.1 dirty: {len(glm_dirty)} tasks")
    print(f"  DS-v4-pro dirty: {len(ds_dirty)} tasks")


if __name__ == "__main__":
    analyze_glm()
