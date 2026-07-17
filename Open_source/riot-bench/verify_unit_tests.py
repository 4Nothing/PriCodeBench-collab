#!/usr/bin/env python3
"""
验证 unit_tests.jsonl 中的测试命令是否能正确执行。

按 run_command 分组执行，解析 embunit 输出中的 "OK (N tests)" 标记，
将每个测试标记为 PASS / BUILD_FAIL / TIMEOUT 等。

输出: verify_results.json + verify_logs/
"""

import json
import subprocess
import os
import re
import hashlib
import sys
import argparse
from pathlib import Path
from collections import defaultdict


def make_log_filename(cmd: str, module: str) -> str:
    h = hashlib.md5(cmd.encode()).hexdigest()[:8]
    safe = module.replace("/", "_").replace("\\", "_")
    return "{}_{}.log".format(safe, h)


def main():
    parser = argparse.ArgumentParser(description="Verify unit tests")
    parser.add_argument("--unit-tests", default=None,
                        help="path to unit_tests.jsonl (default: cwd/unit_tests.jsonl)")
    parser.add_argument("--riot-root", default="/home/carl/RIOT",
                        help="RIOT repo root (default: /home/carl/RIOT)")
    parser.add_argument("--out-dir", default=None,
                        help="output directory (default: cwd)")
    args = parser.parse_args()

    input_file = Path(args.unit_tests or Path.cwd() / "unit_tests.jsonl")
    out_dir = Path(args.out_dir or Path.cwd())
    riot_root = Path(args.riot_root)

    if not input_file.exists():
        print("Error: {} not found".format(input_file))
        sys.exit(1)

    # 读取测试
    tests = []
    with open(input_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    tests.append(json.loads(line))
                except json.JSONDecodeError:
                    continue

    print("Total test entries: {}".format(len(tests)))

    # 按 run_command 分组
    commands = defaultdict(list)
    for test in tests:
        commands[test["run_command"]].append(test)

    unique_commands = list(commands.keys())
    print("Unique run_commands: {}".format(len(unique_commands)))

    # 输出目录
    logs_dir = out_dir / "verify_logs"
    logs_dir.mkdir(parents=True, exist_ok=True)

    test_results = {}

    os.chdir(str(riot_root))

    for i, cmd in enumerate(sorted(unique_commands), 1):
        module_tests = commands[cmd]
        module_name = module_tests[0]["test_base"]
        expected_count = len(module_tests)
        log_file = logs_dir / make_log_filename(cmd, module_name)

        print("\n[{}/{}] {} ({} tests)".format(
            i, len(unique_commands), module_name, expected_count))

        try:
            proc = subprocess.run(
                cmd, shell=True, capture_output=True, text=True, timeout=300
            )
            stdout, stderr, rc = proc.stdout, proc.stderr, proc.returncode

            # 保存日志
            with open(log_file, "w", encoding="utf-8") as f:
                f.write("=== COMMAND ===\n{}\n\n".format(cmd))
                f.write("=== STDOUT ===\n{}\n".format(stdout))
                f.write("=== STDERR ===\n{}\n".format(stderr))
                f.write("=== RC ===\n{}\n".format(rc))

            # 解析 OK (N tests) 行
            ok_match = re.search(r'OK\s*\((\d+)\s+tests?\)', stdout)

            if rc != 0:
                for t in module_tests:
                    test_results[t["test_function"]] = {
                        "status": "BUILD_FAIL",
                        "module": t["test_base"],
                        "trace": "module failed, rc={}, log={}".format(rc, log_file.name),
                    }
                print("  BUILD_FAIL (rc={})".format(rc))
            elif not ok_match:
                for t in module_tests:
                    test_results[t["test_function"]] = {
                        "status": "PARSE_ERROR",
                        "module": t["test_base"],
                        "trace": "no OK(N) line found, log={}".format(log_file.name),
                    }
                print("  PARSE_ERROR (no OK line)")
            else:
                actual_count = int(ok_match.group(1))
                if actual_count >= expected_count:
                    for t in module_tests:
                        test_results[t["test_function"]] = {
                            "status": "PASS",
                            "module": t["test_base"],
                            "trace": "log={}".format(log_file.name),
                        }
                    passed = actual_count
                    total_output = stdout
                    print("  all {} pass ({} expected)".format(passed, expected_count))
                else:
                    for t in module_tests:
                        test_results[t["test_function"]] = {
                            "status": "PARTIAL_PASS",
                            "module": t["test_base"],
                            "trace": "expected {} tests, got {} in output, log={}".format(
                                expected_count, actual_count, log_file.name),
                        }
                    print("  PARTIAL: expected {} tests, got {} in output".format(
                        expected_count, actual_count))

        except subprocess.TimeoutExpired:
            print("  TIMEOUT")
            for t in module_tests:
                test_results[t["test_function"]] = {
                    "status": "TIMEOUT", "module": t["test_base"],
                    "trace": "timeout",
                }
        except Exception as e:
            print("  ERROR: {}".format(e))
            for t in module_tests:
                test_results[t["test_function"]] = {
                    "status": "ERROR", "module": t["test_base"],
                    "trace": "exception={}".format(e),
                }

    # 统计
    statuses = defaultdict(int)
    for v in test_results.values():
        statuses[v["status"]] += 1

    print("\n" + "=" * 50)
    print("Results: {} total".format(len(test_results)))
    for s in ["PASS", "PARTIAL_PASS", "BUILD_FAIL", "TIMEOUT", "ERROR", "PARSE_ERROR"]:
        if statuses[s]:
            print("  {:15s} {}".format(s, statuses[s]))

    # 输出
    out_file = out_dir / "verify_results.json"
    with open(out_file, "w", encoding="utf-8") as f:
        json.dump(test_results, f, indent=2, ensure_ascii=False)
    print("\nResults saved to {}".format(out_file))
    print("Logs saved to {}/".format(logs_dir))


if __name__ == "__main__":
    main()
