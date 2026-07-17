"""
用 tree-sitter AST 解析验证 JSONL 任务文件中的 sut_function → test 映射。

改进点（相比纯文本检测）:
  1. 精确函数名匹配 —— 不会把 bf_set 匹配到 bf_set_all
  2. AST 向上遍历 —— 检测调用是否嵌套在 TEST_ASSERT* 内部
  3. 变量追踪 —— 如果返回值赋给了变量，追踪该变量是否被断言
  4. 副作用断言 —— 如果函数修改了指针参数，检查后续对该变量的断言

用法:
  python3 verify_tasks.py riot_tasks_with_commands.h.jsonl
  python3 verify_tasks.py riot_tasks_with_commands.c.jsonl

输出: 验证后的 JSONL 写入 RIOT/riot-claude/<name>.verified.jsonl
"""

import json
import re
import sys
from pathlib import Path
from tree_sitter import Parser, Language

# ── tree-sitter 初始化 ──────────────────────────────────────
SCRIPT_DIR = Path(__file__).resolve().parent          # riot-bench/
RIOT_ROOT = SCRIPT_DIR.parent / "RIOT"                # RIOT/
BENCH_ROOT = SCRIPT_DIR                               # riot-bench 自身

C_LANG = Language(str(BENCH_ROOT / "build" / "my-languages.so"), "c")
_parser = Parser()
_parser.set_language(C_LANG)

# ── 测试源文件搜索 ──────────────────────────────────────────
TEST_SEARCH_DIRS = [
    RIOT_ROOT / "tests" / "unittests",
    RIOT_ROOT / "tests" / "sys",
    RIOT_ROOT / "tests" / "core",
]

OUTPUT_DIR = RIOT_ROOT / "riot-claude"                 # 输出目录


def get_test_module(run_command):
    m = re.search(r"tests-(\S+)", run_command)
    return m.group(1) if m else None


def find_test_sources(module):
    sources = []
    for base in TEST_SEARCH_DIRS:
        if not base.exists():
            continue
        for d in base.iterdir():
            if d.is_dir() and module in d.name.replace("tests-", ""):
                for f in d.rglob("*.c"):
                    sources.append(str(f))
    return sources


def _clean_identifier(raw):
    """清理标识符: 去掉首尾非标识符字符，修复 tree-sitter 字节偏移。"""
    while raw and not (raw[-1].isalnum() or raw[-1] == "_"):
        raw = raw[:-1]
    while raw and not (raw[0].isalpha() or raw[0] == "_"):
        raw = raw[1:]
    return raw


def _node_text(node, code):
    """提取 AST 节点文本，修复 tree-sitter 对 static/inline 等关键字的 1 字节偏移。"""
    start = node.start_byte
    end = node.end_byte
    while start > 0 and (code[start - 1].isalnum() or code[start - 1] == "_"):
        start -= 1
    return code[start:end]


def _call_name(call_node, code):
    """提取 call_expression 的被调用函数名。"""
    func_node = call_node.child_by_field_name("function")
    if func_node:
        name = _clean_identifier(_node_text(func_node, code))
        if name and name[0].isalpha():
            return name
    return None


# ── AST 查询助手 ────────────────────────────────────────────

def find_all_call_expressions(root):
    """收集 AST 中所有 call_expression 节点。"""
    calls = []

    def walk(n):
        if n.type == "call_expression":
            calls.append(n)
        for child in n.children:
            walk(child)

    walk(root)
    return calls


def _containing_function(node):
    """向上查找包含当前节点的函数定义节点。"""
    while node:
        if node.type == "function_definition":
            return node
        node = node.parent
    return None


def _find_in_subtree(root, node_type, target_text, code):
    """在子树中查找类型为 node_type、文本匹配 target_text 的节点。"""
    results = []

    def walk(n):
        if n.type == node_type:
            if _node_text(n, code) == target_text:
                results.append(n)
        for child in n.children:
            walk(child)

    walk(root)
    return results


def _ancestor_call_name(node, code):
    """向上查找最近的 call_expression 祖先，返回其被调用函数名。"""
    current = node.parent
    while current:
        if current.type == "call_expression":
            return _call_name(current, code)
        current = current.parent
    return None


def _is_assert_call(call_node, code):
    """检查 call_expression 是否是 TEST_ASSERT* 调用。"""
    name = _call_name(call_node, code)
    return name is not None and name.startswith("TEST_ASSERT")


def _var_assigned_from_call(call_node, code):
    """如果 call 的返回值被赋值给了变量，返回变量名和声明节点。

    覆盖两种模式:
      int x = sut_func(...);   → variable_declaration
      x = sut_func(...);       → assignment_expression
    """
    current = call_node

    while current:
        if current.type == "assignment_expression":
            for child in current.children:
                if child.type == "identifier":
                    return _clean_identifier(_node_text(child, code)), current
            break
        if current.type in ("init_declarator",):
            for child in current.children:
                if child.type == "identifier":
                    return _clean_identifier(_node_text(child, code)), current
            break
        current = current.parent

    return None, None


def _var_asserted_in_function(func_node, var_name, code):
    """检查变量 var_name 是否在函数体内的 TEST_ASSERT* 调用中被使用。"""
    body = func_node.child_by_field_name("body")
    if body is None:
        return False

    assert_calls = []

    def walk(n):
        if n.type == "call_expression":
            name = _call_name(n, code)
            if name and name.startswith("TEST_ASSERT"):
                assert_calls.append(n)
        for child in n.children:
            walk(child)

    walk(body)

    for ac in assert_calls:
        if var_name in _node_text(ac, code):
            return True
    return False


# ── 核心验证逻辑 ────────────────────────────────────────────

def check_sut_asserted(sut_function, test_source_files):
    """检查 sut_function 是否被测试源文件中的断言验证。

    三种验证模式:
      A. 直接断言: sut_func(...) 作为 TEST_ASSERT* 的参数
      B. 变量追踪: var = sut_func(...);  // ... TEST_ASSERT*(..., var, ...)
      C. 邻行副作用: 函数调用后 5 行内出现 TEST_ASSERT
                     (覆盖 void 函数通过指针修改状态的模式)

    返回: (found: bool, file: str, detail: str)
    """
    for src_file in test_source_files:
        try:
            code = Path(src_file).read_text(errors="replace")
        except OSError:
            continue

        tree = _parser.parse(bytes(code, "utf8"))
        root = tree.root_node
        all_calls = find_all_call_expressions(root)

        sut_calls = [c for c in all_calls if _call_name(c, code) == sut_function]
        if not sut_calls:
            continue

        for call_node in sut_calls:
            # ── 模式 A: 直接断言 ──
            ancestor = _ancestor_call_name(call_node, code)
            if ancestor and ancestor.startswith("TEST_ASSERT"):
                return True, src_file, f"direct: nested in {ancestor}"

            # ── 模式 B: 返回值被赋值后被断言 ──
            var_name, _ = _var_assigned_from_call(call_node, code)
            if var_name:
                func_node = _containing_function(call_node)
                if func_node and _var_asserted_in_function(func_node, var_name, code):
                    return True, src_file, f"variable: '{var_name}' asserted later"

            # ── 模式 C: 邻行副作用 ──
            call_line = call_node.start_point[0]
            lines = code.split("\n")
            for offset in range(1, 6):
                check_line = call_line + offset
                if check_line < len(lines) and "TEST_ASSERT" in lines[check_line]:
                    return True, src_file, f"nearby: assertion within {offset} lines"

    return False, "", ""


def is_called(sut_function, test_source_files):
    """检查函数是否至少被调用过（仅用于未通过断言检查时的 fallback）。"""
    for src_file in test_source_files:
        try:
            code = Path(src_file).read_text(errors="replace")
        except OSError:
            continue
        tree = _parser.parse(bytes(code, "utf8"))
        for call in find_all_call_expressions(tree.root_node):
            if _call_name(call, code) == sut_function:
                return True, src_file
    return False, ""


# ── 主流程 ──────────────────────────────────────────────────

def verify(input_path):
    entries = []
    with open(input_path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            entries.append(json.loads(line))

    module_cache = {}
    asserted = []
    called_only = []
    not_found = []

    for entry in entries:
        task_id = entry["task_id"]
        sut_func = entry["sut_function"]
        run_cmd = entry["run_command"]
        unit_test = entry.get("unit_test", "")
        source_path = entry.get("source_path", "")

        module = get_test_module(run_cmd)
        if module not in module_cache:
            module_cache[module] = find_test_sources(module)
        sources = module_cache[module]

        has_assert, src_file, detail = check_sut_asserted(sut_func, sources)
        if has_assert:
            asserted.append(entry)
        else:
            has_call, call_file = is_called(sut_func, sources)
            if has_call:
                called_only.append(
                    (task_id, sut_func, unit_test, module, source_path, call_file)
                )
            else:
                not_found.append(
                    (task_id, sut_func, unit_test, module, source_path)
                )

    total = len(entries)
    pct = lambda n: f"{100 * n / total:.1f}%"
    print(f"Total entries:            {total}")
    print(f"With assertions:          {len(asserted)} ({pct(len(asserted))})")
    print(f"Only called (excluded):   {len(called_only)} ({pct(len(called_only))})")
    print(f"Not called (excluded):    {len(not_found)} ({pct(len(not_found))})")

    if called_only:
        print(f"\n--- Excluded: called but no assertion ---")
        for tid, func, test, mod, src, f in called_only:
            print(f"  task {tid}: {func} -> {test} [{mod}]")

    if not_found:
        print(f"\n--- Excluded: not called ---")
        for tid, func, test, mod, src in not_found:
            print(f"  task {tid}: {func} -> {test} [{mod}]")

    output_path = OUTPUT_DIR / Path(input_path).with_suffix(".verified.jsonl").name
    output_path.parent.mkdir(exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        for entry in asserted:
            f.write(json.dumps(entry, ensure_ascii=False) + "\n")
    print(f"\nVerified output: {output_path}")

    return asserted, called_only, not_found


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 verify_tasks.py <task_file.jsonl>")
        sys.exit(1)
    verify(sys.argv[1])
