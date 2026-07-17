#!/usr/bin/env python3
"""
从 RIOT 源码中提取被测试函数定义，生成 LLM 补全任务。

流程:
1. 读取 unit_tests.jsonl → 获取测试函数名和测试文件
2. 用 tree-sitter 解析测试文件 → 提取被测函数调用的所有函数名 (SUT candidates)
3. 在 RIOT 源码目录 (sys/ drivers/ core/ cpu/) 中查找这些函数的定义
4. 按目录优先级选择最合适的定义 (sys/ > drivers/ > core/ > cpu/ > 其他)
5. 为每个函数生成 masked_code + oracle → 输出任务文件
"""

import json
import re
import sys
import argparse
from pathlib import Path

from tree_sitter import Parser, Language
from mask_engine import mask_function


C_LANG = Language(Path(__file__).parent / "build" / "my-languages.so", "c")
parser = Parser()
parser.set_language(C_LANG)

# 函数定义来源的目录优先级 (数字越大优先级越高)
# sys/ 中的实现最可能是被测的真实代码
SOURCE_PRIORITY = {
    "sys":     5,
    "drivers": 4,
    "core":    3,
    "cpu":     2,
}

DEFAULT_PRIORITY = 1

# 要排除的顶层目录 (匹配 repo_root 下的相对路径)
EXCLUDED_DIRS = {"tests", "build", "dist", "riot-bench", "bin", ".git"}

# 测试框架函数 / C 标准库 / 常见过滤
FRAMEWORK_FUNCS = {
    "TEST_ASSERT", "TEST_ASSERT_EQUAL_INT", "TEST_ASSERT_EQUAL_STRING",
    "TEST_ASSERT_NOT_NULL", "TEST_ASSERT_NULL", "TESTS_RUN",
    "TEST_ASSERT_EQUAL", "TEST_ASSERT_PARAM", "TEST_ASSERT_EQUAL_BYTES",
    "strlen", "sizeof", "memcmp", "memcpy", "memset", "printf",
    "calloc", "malloc", "free", "abort", "exit", "strcmp", "strcpy",
    "sprintf", "snprintf", "fprintf", "puts", "putchar", "getchar",
    "atoi", "atol", "atof", "strtol", "strtoul", "strtod",
    "memchr", "memmove", "strcat", "strncat", "strncpy",
    "strchr", "strrchr", "strstr", "strtok", "strerror", "perror",
}

C_KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while", "inline", "restrict", "_Bool", "_Complex", "_Imaginary",
}


def is_excluded(rel_path: str) -> bool:
    """检查相对路径是否应排除 (tests/, build/ 等)"""
    top = rel_path.split("/", 1)[0] if "/" in rel_path else rel_path
    return top in EXCLUDED_DIRS


def path_priority(rel_path: str) -> int:
    """返回路径优先级: sys/=5, drivers/=4, core/=3, cpu/=2, 其他=1"""
    top = rel_path.split("/", 1)[0] if "/" in rel_path else rel_path
    return SOURCE_PRIORITY.get(top, DEFAULT_PRIORITY)


def clean_identifier(raw: str) -> str:
    """清理 identifier: 去尾随 ( 等非标识符字符，确保是有效的 C 名"""
    # 移除尾随的非标识符字符 (tree-sitter 对某些 static inline 有偏移问题)
    while raw and not (raw[-1].isalnum() or raw[-1] == "_"):
        raw = raw[:-1]
    return raw


def is_valid_func_name(name: str) -> bool:
    """检查是否是有效的 C 函数名 (排除宏 ALL_CAPS、解析伪影等)"""
    if not name or not name[0].isalpha() or name.startswith("_"):
        return False
    # 排除全大写的宏名 (纯大写或带下划线的大写)
    if name.isupper() and len(name) > 1:
        return False
    # 确保只含有效字符
    return all(c.isalnum() or c == "_" for c in name)


def extract_function_body(node, code: str) -> str:
    """提取函数定义全文，修复 tree-sitter 对 static 等关键字的偏移问题"""
    start = node.start_byte
    end = node.end_byte

    # tree-sitter 有时会丢失 static 的首字母 (tatic 而非 static)
    # 往回退过连续单词字符来恢复
    while start > 0 and (code[start - 1].isalnum() or code[start - 1] == "_"):
        start -= 1

    return code[start:end]


# 函数签名中应跳过的关键字/属性 (第一个匹配的非跳过词才是函数名)
_SIG_SKIP = {"if", "for", "while", "switch", "return", "__attribute__", "__attribute"}


def get_func_name(node, code: str):
    """从函数签名中用正则提取函数名 (绕过 tree-sitter identifier 偏移 bug)"""
    body = extract_function_body(node, code)
    # 只取函数签名 ({ 之前)
    sig = body.split("{")[0] if "{" in body else body
    # 剔除 __attribute__((...)) 以避免内部词误匹配
    import re
    sig = re.sub(r"__attribute__\s*\(\([^)]*\)\)", "", sig)

    for m in re.finditer(r"(\w+)\s*\(", sig):
        name = clean_identifier(m.group(1))
        if name and name not in _SIG_SKIP:
            return name
    return None


def extract_functions(code: str):
    """用 tree-sitter 从源码中提取所有函数定义 (无重复)"""
    tree = parser.parse(bytes(code, "utf8"))
    results = []
    seen = set()

    def collect_func(node):
        if node.type == "function_definition":
            name = get_func_name(node, code)
            body = extract_function_body(node, code)
            if name and is_valid_func_name(name):
                key = (name, node.start_byte)
                if key not in seen:
                    seen.add(key)
                    results.append({
                        "name": name,
                        "start": node.start_byte,
                        "end": node.end_byte,
                        "body": body,
                    })
        for child in node.children:
            collect_func(child)

    collect_func(tree.root_node)
    return results


def extract_called_functions(code: str):
    """用 tree-sitter AST 从测试文件中提取所有被调用的函数名"""
    tree = parser.parse(bytes(code, "utf8"))

    called = set()
    def walk(node):
        if node.type == "call_expression":
            func_node = node.child_by_field_name("function")
            if func_node and func_node.type == "identifier":
                start = func_node.start_byte
                end = func_node.end_byte
                # tree-sitter 对某些标识符有 1 字节偏移 (同 extract_function_body)
                while start > 0 and (code[start - 1].isalnum()
                                     or code[start - 1] == "_"):
                    start -= 1
                name = clean_identifier(code[start:end])
                if (is_valid_func_name(name)
                        and name not in FRAMEWORK_FUNCS
                        and not name.startswith("test_")):
                    called.add(name)
        for child in node.children:
            walk(child)

    walk(tree.root_node)
    return called


def find_sut_functions(repo_root, unit_tests):
    """从测试文件中提取 SUT (被测试函数) 名"""
    repo_root = Path(repo_root)
    test_to_sut = {}
    all_sut = set()

    for test in unit_tests:
        test_func = test.get("test_function")
        test_file = test.get("test_file")
        if not test_func or not test_file:
            continue

        path = repo_root / test_file
        if not path.exists():
            continue

        try:
            code = path.read_text(errors="replace")
            funcs = extract_called_functions(code)
            funcs = {f for f in funcs if f and f[0].isalpha() and not f.startswith("_")}
            if funcs:
                test_to_sut[test_func] = funcs
                all_sut.update(funcs)
        except Exception:
            continue

    return test_to_sut, all_sut


def find_function_definitions(repo_root, func_names):
    """
    在所有源码目录中查找函数定义，按路径优先级选择最佳定义。

    Returns:
        dict: {func_name: {"file": rel_path, "body": str, "start": int}}
    """
    repo_root = Path(repo_root)
    # {func_name: (priority, func_info)}
    best = {}

    for ext in ("*.c", "*.h", "*.S", "*.cpp"):
        for file in repo_root.rglob(ext):
            rel = file.relative_to(repo_root).as_posix()
            if is_excluded(rel):
                continue

            try:
                text = file.read_text(errors="replace")
            except Exception:
                continue

            for func in extract_functions(text):
                name = func["name"]
                if name not in func_names:
                    continue

                prio = path_priority(rel)
                prev_prio, _ = best.get(name, (0, None))
                if prio > prev_prio:
                    best[name] = (prio, {
                        "file": rel,
                        "body": func["body"],
                        "start": func["start"],
                    })

    # 剥掉优先级，只返回信息
    return {name: info for name, (_, info) in best.items()}


def select_best_test(func_name, test_to_sut, test_commands):
    """为被测函数选择最相关的测试函数和命令"""
    relevant = []
    for test_func, sut_funcs in test_to_sut.items():
        if func_name in sut_funcs:
            score = 2 if func_name in test_func else 1
            relevant.append((score, test_func))

    if not relevant:
        return None, None

    relevant.sort(key=lambda x: -x[0])
    best = relevant[0][1]
    return best, test_commands.get(best)


def build(repo_root, unit_tests_path, out_prefix):
    repo_root = Path(repo_root)
    out_stem = Path(out_prefix)

    # 读取 unit_tests.jsonl
    unit_tests = []
    test_commands = {}

    with open(unit_tests_path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                test = json.loads(line)
                unit_tests.append(test)
                tf = test.get("test_function")
                cmd = test.get("run_command")
                if tf and cmd:
                    test_commands[tf] = cmd
            except json.JSONDecodeError:
                continue

    print(f"Loaded {len(unit_tests)} unit tests, {len(test_commands)} commands")

    # 提取 SUT 函数名
    test_to_sut, sut_funcs = find_sut_functions(repo_root, unit_tests)
    print(f"Found {len(sut_funcs)} unique SUT function names from test files")

    # 按优先级查找函数定义
    func_locations = find_function_definitions(repo_root, sut_funcs)
    print(f"Found definitions for {len(func_locations)} functions")

    # 输出未找到定义的函数
    missing = sut_funcs - set(func_locations.keys())
    if missing:
        print(f"Missing definitions for {len(missing)} functions:")
        for fn in sorted(missing)[:15]:
            print(f"  - {fn}")
        if len(missing) > 15:
            print(f"  ... and {len(missing) - 15} more")

    # 确保 oracles 目录存在
    out_dir = Path(out_prefix).parent
    oracles_dir = out_dir / "oracles"
    oracles_dir.mkdir(exist_ok=True)

    # 按 .c / .h 分流
    c_out = Path(out_prefix + ".c.jsonl")
    h_out = Path(out_prefix + ".h.jsonl")

    # 第一遍：收集并按源类型分流
    def make_task(func_name, func_info):
        masked, target = mask_function(func_info["body"])
        if masked is None:
            return None
        unit_test, run_command = select_best_test(
            func_name, test_to_sut, test_commands
        )
        return {
            "source_path": func_info["file"],
            "sut_function": func_name,
            "masked_code": masked,
            "unit_test": unit_test,
            "run_command": run_command,
            "func_body": func_info["body"],
        }

    c_tasks = []
    h_tasks = []
    for func_name, func_info in sorted(func_locations.items(), key=lambda x: x[0]):
        t = make_task(func_name, func_info)
        if t is None:
            print(f"  WARN: cannot mask {func_name}, skipping")
            continue
        if func_info["file"].endswith(".h"):
            h_tasks.append(t)
        else:
            c_tasks.append(t)

    # 第二遍：分别写入各文件，独立编号，oracle 扩展名匹配源文件类型
    def write_tasks(tasks, suffix, path):
        with open(path, "w", encoding="utf-8") as f:
            for tid, task in enumerate(tasks, 1):
                oracle_name = f"{tid}.{suffix}"
                oracle_path = oracles_dir / oracle_name
                oracle_path.write_text(task["func_body"], encoding="utf-8")

                record = {
                    "task_id": str(tid),
                    "source_path": task["source_path"],
                    "sut_function": task["sut_function"],
                    "oracle": str(oracle_path.relative_to(out_dir)),
                    "masked_code": task["masked_code"],
                    "unit_test": task["unit_test"],
                    "run_command": task["run_command"],
                }
                f.write(json.dumps(record) + "\n")

    write_tasks(c_tasks, "c", c_out)
    write_tasks(h_tasks, "h", h_out)
    print(f"Generated {len(c_tasks)} C-file tasks → {c_out}")
    print(f"Generated {len(h_tasks)} header tasks → {h_out}")


def main():
    parser = argparse.ArgumentParser(
        description="Build RIOT code completion benchmark dataset"
    )
    parser.add_argument("--repo", required=True, help="RIOT repo root")
    parser.add_argument(
        "--unit-tests",
        default=None,
        help="unit_tests.jsonl path (default: <cwd>/unit_tests.jsonl)",
    )
    parser.add_argument("--out-prefix", required=True, help="output prefix (e.g. riot_tasks → riot_tasks.c.jsonl + riot_tasks.h.jsonl)")
    args = parser.parse_args()

    # 默认 unit_tests.jsonl 在当前目录
    unit_tests_path = args.unit_tests or str(
        Path.cwd() / "unit_tests.jsonl"
    )

    build(args.repo, unit_tests_path, args.out_prefix)


if __name__ == "__main__":
    main()
