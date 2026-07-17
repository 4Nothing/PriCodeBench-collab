#!/usr/bin/env python3
"""
从 RIOT 单元测试源文件中提取测试信息，生成 unit_tests.jsonl

流程:
1. 用 rg 在 tests/unittests/ 下搜索所有 new_TestFixture(test_...) 注册
2. 确定 test_base (tests-core, tests-checksum 等测试模块)
3. 找到每个测试函数的定义文件
4. 组装 run_command，去重排序输出 JSONL

依赖: ripgrep (rg)
"""

import json
import hashlib
import re
import subprocess
import sys
from pathlib import Path
from collections import Counter


def normalize(name: str) -> str:
    """test_pkt_len__1_elem → pkt_len"""
    if name.startswith("test_"):
        name = name[5:]
    return name.split("__")[0]


def find_test_definition(rg_result_lines, func_name):
    """在 rg 搜索结果中查找测试函数定义所在的行"""
    for line in rg_result_lines:
        parts = line.split(":", 2)
        if len(parts) < 3:
            continue
        fp, ln, rest = parts
        # 匹配 static void test_xxx(void) 这种函数签名
        m = re.search(rf"(?:^|\s)(?:static\s+)?void\s+{re.escape(func_name)}\s*\(", rest)
        if m:
            return fp
    return None


def main():
    if len(sys.argv) < 2:
        print("Usage: extract_unit_tests.py <RIOT_root> [output_path]")
        sys.exit(1)

    repo_root = Path(sys.argv[1])
    test_root = repo_root / "tests" / "unittests"
    output_file = Path(sys.argv[2]) if len(sys.argv) > 2 else Path.cwd() / "unit_tests.jsonl"

    if not test_root.is_dir():
        print(f"Error: {test_root} not found")
        sys.exit(1)

    print("Scanning test fixtures with rg...")

    # Step 1: 从 new_TestFixture 注册中找到所有测试函数名（这是权威来源）
    try:
        fixture_result = subprocess.run(
            ["rg", "-n", r"new_TestFixture\(test_(\w+)\)", str(test_root),
             "--type", "c", "--no-heading"],
            capture_output=True, text=True, timeout=120
        )
    except FileNotFoundError:
        print("Error: rg (ripgrep) not found. Please install it first.")
        sys.exit(1)

    # {func_name: [(filepath, line)]}
    registered = {}
    for line in fixture_result.stdout.strip().split("\n"):
        if not line:
            continue
        parts = line.split(":", 2)
        if len(parts) < 3:
            continue
        fp, ln, rest = parts
        # 跳过注释行（// 或 /* ... */）
        stripped = rest.strip()
        if stripped.startswith("//") or stripped.startswith("/*"):
            continue
        m = re.search(r"new_TestFixture\(test_(\w+)\)", stripped)
        if m:
            name = "test_" + m.group(1)
            registered.setdefault(name, []).append(fp)

    if not registered:
        print("Error: no test fixtures found")
        sys.exit(1)

    # Step 2: 找到每个测试函数的定义文件
    # 预搜索所有 test_xxx 函数定义
    try:
        def_result = subprocess.run(
            ["rg", "-n", r"(?:^|\s)(?:static\s+)?void\s+test_\w+\s*\(", str(test_root),
             "--type", "c", "--no-heading"],
            capture_output=True, text=True, timeout=120
        )
    except FileNotFoundError:
        print("Error: rg (ripgrep) not found.")
        sys.exit(1)

    def_lines = def_result.stdout.strip().split("\n")

    # Step 3: 组装测试条目
    all_tests = []
    for func_name in sorted(registered.keys()):
        src_file = find_test_definition(def_lines, func_name)

        if not src_file:
            # 如果找不到定义，用注册文件作为 fallback
            src_file = registered[func_name][0]

        src_path = Path(src_file)
        try:
            rel = src_path.relative_to(test_root)
        except ValueError:
            continue
        if not rel.parts or not rel.parts[0].startswith("tests-"):
            continue

        test_base = rel.parts[0]
        test_file = str(src_path.relative_to(repo_root))

        # 过滤构建产物路径
        if "bin/" in test_file:
            continue

        test_id = hashlib.md5(
            "{}:{}:{}".format(func_name, test_base, test_file).encode()
        ).hexdigest()[:8]
        run_command = (
            "cd tests/unittests && "
            "make BOARD=native64 {} -j && "
            "./bin/native64/tests_unittests.elf".format(test_base)
        )
        all_tests.append({
            "test_id": test_id,
            "test_function": func_name,
            "normalized_function": normalize(func_name),
            "test_file": test_file,
            "test_base": test_base,
            "candidate_files": [test_file],
            "num_candidate_files": 1,
            "suite_filterable": True,
            "run_command": run_command,
        })

    # 去重
    seen = set()
    unique = []
    for t in all_tests:
        key = (t["test_function"], t["test_base"], t["test_file"])
        if key not in seen:
            seen.add(key)
            unique.append(t)

    unique.sort(key=lambda x: x["test_function"])

    # 输出
    output_file = Path(output_file)
    output_file.parent.mkdir(parents=True, exist_ok=True)
    with open(output_file, "w", encoding="utf-8") as f:
        for t in unique:
            f.write(json.dumps(t, ensure_ascii=False) + "\n")

    suites = Counter(t["test_base"] for t in unique)
    print("Extracted {} unit tests from {} suites → {}".format(
        len(unique), len(suites), output_file))
    print("\nTop suites:")
    for s, c in suites.most_common(10):
        print("  {:25s} {:>4d} tests".format(s, c))


if __name__ == "__main__":
    main()
