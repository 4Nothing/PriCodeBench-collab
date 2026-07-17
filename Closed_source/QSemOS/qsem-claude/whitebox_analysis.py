"""
White-box failure analysis for QSemOS benchmark.

For each failed task, extracts Claude's implementation from trajectory log,
compares with oracle ground truth at AST level, and produces a 3-level
fine-grained failure classification.

Output:
  1. Per-task detailed analysis (whitebox_report.json)
  2. Module x ErrorType cross table
  3. Summary statistics for writing RQs
"""
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Optional

from tree_sitter import Language, Parser
import tree_sitter_c

# ---------------------------------------------------------------------------
# paths
# ---------------------------------------------------------------------------
SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
ORACLES_DIR = REPO_ROOT.parent / "oracles"
RESULTS_FILE = SCRIPT_DIR / "results" / "results.jsonl"
TRAJECTORY_DIR = SCRIPT_DIR / "trajectory"
OUTPUT_FILE = SCRIPT_DIR / "results" / "whitebox_report.json"

_C_LANG = Language(tree_sitter_c.language())


# ---------------------------------------------------------------------------
# tree-sitter helpers (mirrors mask.py)
# ---------------------------------------------------------------------------
def _char_offset(text: str, byte_offset: int) -> int:
    return len(text.encode("utf-8")[:byte_offset].decode("utf-8", "replace"))


def _find_declarator_identifier(node):
    if node.type == "identifier":
        return node
    child = node.child_by_field_name("declarator")
    if child:
        return _find_declarator_identifier(child)
    for c in node.children:
        result = _find_declarator_identifier(c)
        if result is not None:
            return result
    return None


def find_function(source: str, fn_name: str):
    parser = Parser(_C_LANG)
    tree = parser.parse(bytes(source, "utf8"))
    root = tree.root_node
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type == "function_definition":
            declarator = node.child_by_field_name("declarator")
            if declarator:
                id_node = _find_declarator_identifier(declarator)
                if id_node is not None:
                    if source[_char_offset(source, id_node.start_byte):
                              _char_offset(source, id_node.end_byte)] == fn_name:
                        return node
        stack.extend(node.children)
    return None


def extract_body(source: str, fn_name: str) -> Optional[str]:
    fn = find_function(source, fn_name)
    if fn is None:
        return None
    body = fn.child_by_field_name("body")
    if body is None:
        return None
    start = _char_offset(source, body.start_byte + 1)
    end = _char_offset(source, body.end_byte - 1)
    return source[start:end]


# ---------------------------------------------------------------------------
# AST collectors
# ---------------------------------------------------------------------------
def collect_calls(source: str) -> set:
    """Extract all function call names from source code."""
    parser = Parser(_C_LANG)
    tree = parser.parse(bytes(source, "utf8"))
    root = tree.root_node
    calls = set()
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type == "call_expression":
            fn_node = node.child_by_field_name("function")
            if fn_node:
                name = source[_char_offset(source, fn_node.start_byte):
                              _char_offset(source, fn_node.end_byte)]
                # skip indirect calls (-> or .)
                if not any(c in name for c in ("->", ".")):
                    calls.add(name)
        stack.extend(node.children)
    return calls


def collect_field_accesses(source: str) -> set:
    """Extract all struct field accesses (a->b or a.b)."""
    parser = Parser(_C_LANG)
    tree = parser.parse(bytes(source, "utf8"))
    root = tree.root_node
    fields = set()
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type == "field_expression":
            field = node.child_by_field_name("field")
            if field:
                fields.add(source[_char_offset(source, field.start_byte):
                                  _char_offset(source, field.end_byte)])
        stack.extend(node.children)
    return fields


def collect_control_nodes(source: str) -> dict:
    """Count control flow constructs."""
    parser = Parser(_C_LANG)
    tree = parser.parse(bytes(source, "utf8"))
    root = tree.root_node
    counts = Counter()
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type in ("if_statement", "else_clause", "switch_statement",
                         "while_statement", "for_statement", "do_statement",
                         "return_statement", "goto_statement"):
            counts[node.type] += 1
        stack.extend(node.children)
    return dict(counts)


def collect_macros(source: str) -> set:
    """Extract macro names used (all-caps identifiers)."""
    parser = Parser(_C_LANG)
    tree = parser.parse(bytes(source, "utf8"))
    root = tree.root_node
    macros = set()
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type == "identifier":
            name = source[_char_offset(source, node.start_byte):
                          _char_offset(source, node.end_byte)]
            if name.isupper() and "_" in name:
                macros.add(name)
        stack.extend(node.children)
    return macros


# ---------------------------------------------------------------------------
# difference analysis
# ---------------------------------------------------------------------------
def diff_calls(claude_calls: set, oracle_calls: set) -> dict:
    """Compare function calls between Claude and oracle."""
    return {
        "only_claude": sorted(claude_calls - oracle_calls),
        "only_oracle": sorted(oracle_calls - claude_calls),
        "shared": sorted(claude_calls & oracle_calls),
        "claude_count": len(claude_calls),
        "oracle_count": len(oracle_calls),
        "jaccard": (len(claude_calls & oracle_calls) /
                    len(claude_calls | oracle_calls)
                    if (claude_calls | oracle_calls) else 1.0),
    }


def diff_fields(claude_fields: set, oracle_fields: set) -> dict:
    return {
        "only_claude": sorted(claude_fields - oracle_fields),
        "only_oracle": sorted(oracle_fields - claude_fields),
        "jaccard": (len(claude_fields & oracle_fields) /
                    len(claude_fields | oracle_fields)
                    if (claude_fields | oracle_fields) else 1.0),
    }


def diff_macros(claude_macros: set, oracle_macros: set) -> dict:
    return {
        "only_claude": sorted(claude_macros - oracle_macros),
        "only_oracle": sorted(oracle_macros - claude_macros),
    }


# ---------------------------------------------------------------------------
# line-level diff (character-based, no git dep)
# ---------------------------------------------------------------------------
def simple_diff_lines(a_lines: list, b_lines: list) -> list:
    """Very simple line-level diff: returns list of (op, line) tuples."""
    import difflib
    result = []
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(
            None, a_lines, b_lines).get_opcodes():
        if tag == "equal":
            continue
        elif tag == "replace":
            for l in a_lines[i1:i2]:
                result.append(("-", l))
            for l in b_lines[j1:j2]:
                result.append(("+", l))
        elif tag == "delete":
            for l in a_lines[i1:i2]:
                result.append(("-", l))
        elif tag == "insert":
            for l in b_lines[j1:j2]:
                result.append(("+", l))
    return result


# ---------------------------------------------------------------------------
# classification engine
# ---------------------------------------------------------------------------
def classify_failure(error_cat: str, claude_source: str,
                     oracle_source: str, sut_function: str,
                     source_path: str, claude_body: str,
                     oracle_body: str, test_output: str) -> dict:
    """
    Return 3-level classification:
      L1: error category
      L2: sub-category
      L3: specific root cause
    """
    L1 = error_cat  # compile_error / crash / test_failure

    claude_calls = collect_calls(claude_source)
    oracle_calls = collect_calls(oracle_source)
    claude_fields = collect_field_accesses(claude_source)
    oracle_fields = collect_field_accesses(oracle_source)
    claude_macros = collect_macros(claude_source)
    oracle_macros = collect_macros(oracle_source)
    call_diff = diff_calls(claude_calls, oracle_calls)
    field_diff = diff_fields(claude_fields, oracle_fields)
    macro_diff = diff_macros(claude_macros, oracle_macros)
    claude_ctrl = collect_control_nodes(claude_source)
    oracle_ctrl = collect_control_nodes(oracle_source)

    # ---- L2 classification ----
    L2 = ""
    L3_detail = []

    if error_cat == "compile_error":
        L2, L3_detail = _classify_compile(call_diff, macro_diff, test_output)

    elif error_cat == "crash":
        L2, L3_detail = _classify_crash(
            claude_body, oracle_body, call_diff, field_diff,
            macro_diff, claude_ctrl, oracle_ctrl, sut_function, source_path,
            claude_source, oracle_source,
        )

    elif error_cat == "test_failure":
        L2, L3_detail = _classify_logic(
            claude_body, oracle_body, call_diff, field_diff,
            macro_diff, claude_ctrl, oracle_ctrl, sut_function, claude_source,
            oracle_source, test_output,
        )

    return {
        "L1": L1,
        "L2": L2,
        "L3_detail": L3_detail,
        "call_diff": call_diff,
        "field_diff": field_diff,
        "macro_diff": macro_diff,
        "claude_control": claude_ctrl,
        "oracle_control": oracle_ctrl,
    }


def _classify_compile(call_diff: dict, macro_diff: dict,
                      test_output: str) -> tuple:
    """Classify compile errors."""
    detail = []
    L2 = "hallucinated_api"

    # Check for specific error patterns in test output
    if "undefined reference to" in test_output:
        # Extract the missing symbol
        refs = re.findall(r"undefined reference to `(\w+)'", test_output)
        if refs:
            detail.append(f"undefined_reference: {', '.join(refs)}")
            L2 = "undefined_reference"

    if "implicit declaration" in test_output:
        implicit = re.findall(r"implicit declaration of function '(\w+)'",
                              test_output)
        if implicit:
            detail.append(f"implicit_declaration: {', '.join(implicit)}")
            if L2 == "hallucinated_api":
                L2 = "implicit_declaration"

    # Check if Claude used symbols not in oracle
    extra_calls = call_diff.get("only_claude", [])
    if extra_calls:
        # Filter to likely hallucinated (no lowercase-only or short names)
        suspicious = [c for c in extra_calls
                      if not c.startswith("Os") and not c.startswith("PRT_")]
        if suspicious:
            detail.append(f"suspicious_hallucinated_calls: {', '.join(suspicious)}")

    # Check macros
    extra_macros = macro_diff.get("only_claude", [])
    if extra_macros:
        detail.append(f"extra_macros_not_in_oracle: {', '.join(extra_macros)}")

    if not detail:
        detail.append("unknown_compile_error")

    return L2, detail


def _classify_crash(claude_body: str, oracle_body: str,
                    call_diff: dict, field_diff: dict,
                    macro_diff: dict, claude_ctrl: dict, oracle_ctrl: dict,
                    sut_function: str, source_path: str,
                    claude_full: str, oracle_full: str) -> tuple:
    """Classify crash (segfault/abort/panic) root causes."""
    detail = []
    L2 = "unknown_crash"

    # Heuristic 1: missing lock/unlock calls
    claude_calls = set(call_diff.get("shared", [])) | set(call_diff.get("only_claude", []))
    oracle_calls_set = set(call_diff.get("shared", [])) | set(call_diff.get("only_oracle", []))

    lock_keywords = ["Lock", "Unlock", "SpinLock", "SpinUnlock", "IntLock", "IntRestore"]
    missing_locks = [c for c in oracle_calls_set - claude_calls
                     if any(kw in c for kw in lock_keywords)]
    if missing_locks:
        detail.append(f"missing_lock_calls: {', '.join(missing_locks)}")
        L2 = "missing_synchronization"

    # Heuristic 2: pointer null check missing
    oracle_lines = [l.strip() for l in oracle_body.splitlines() if l.strip()]
    claude_lines = [l.strip() for l in claude_body.splitlines() if l.strip()]
    null_check_pattern = re.compile(r'(if|assert)\s*\(.*(==\s*NULL|!=\s*NULL|==\s*0|!\w+)')
    oracle_null_checks = [l for l in oracle_lines if null_check_pattern.search(l)]
    claude_null_checks = [l for l in claude_lines if null_check_pattern.search(l)]
    if len(oracle_null_checks) > len(claude_null_checks):
        missing = len(oracle_null_checks) - len(claude_null_checks)
        detail.append(f"missing_null_checks: {missing} null-check(s) in oracle not in Claude ({len(oracle_null_checks)} vs {len(claude_null_checks)})")
        if L2 == "unknown_crash":
            L2 = "missing_null_check"

    # Heuristic 3: linked list / data structure operation errors
    list_ops = ["LIST_FOR_EACH", "ListDelete", "ListAdd", "ListInsert",
                "OsTskReadyAdd", "OsTskReadyDel", "TSK_STATUS_CLEAR",
                "TSK_STATUS_SET"]
    oracle_list = [c for c in oracle_calls_set if any(op in c for op in list_ops)]
    claude_list = [c for c in claude_calls if any(op in c for op in list_ops)]
    if oracle_list:
        missing = set(oracle_list) - set(claude_list)
        extra = set(claude_list) - set(oracle_list)
        if missing or extra:
            if missing:
                detail.append(f"missing_list_queue_ops: {', '.join(sorted(missing))}")
            if extra:
                detail.append(f"extra_list_queue_ops: {', '.join(sorted(extra))}")
            if L2 == "unknown_crash":
                L2 = "data_structure_misuse"

    # Heuristic 4: bitmask / signal mask errors
    if "signal" in source_path.lower() or "sigMask" in oracle_body:
        mask_ops = ["sigMask", "sigPending", "sigWaitMask"]
        oracle_mask = [c for c in oracle_calls_set if any(m in c for m in mask_ops)]
        claude_mask = [c for c in claude_calls if any(m in c for m in mask_ops)]
        if set(oracle_mask) != set(claude_mask):
            detail.append(f"signal_mask_mismatch: oracle={oracle_mask}, claude={claude_mask}")
            if L2 == "unknown_crash":
                L2 = "bitmask_error"

    # Heuristic 5: extra fields accessed that don't exist
    extra_fields = field_diff.get("only_claude", [])
    if extra_fields:
        detail.append(f"extra_field_accesses: {', '.join(extra_fields[:5])}")

    # Heuristic 6: control flow missing
    for key in ("if_statement", "while_statement", "for_statement"):
        if oracle_ctrl.get(key, 0) > claude_ctrl.get(key, 0):
            diff = oracle_ctrl.get(key, 0) - claude_ctrl.get(key, 0)
            detail.append(f"missing_{key}s: oracle has {oracle_ctrl.get(key,0)}, Claude has {claude_ctrl.get(key,0)} (Δ={diff})")

    # Heuristic 7: stack/context operations (OsTskContextInit)
    if "OsTskContextInit" in oracle_calls_set and "OsTskContextInit" not in claude_calls:
        detail.append("missing_OsTskContextInit: critical for scheduler context switch")
        if L2 == "unknown_crash":
            L2 = "missing_context_switch"

    # Heuristic 8: task status operations
    status_ops = ["TSK_STATUS_CLEAR", "TSK_STATUS_SET",
                   "TSK_IS_UNUSED", "TSK_IS_READY"]
    oracle_status = [c for c in oracle_calls_set if any(op in c for op in status_ops)]
    claude_status = [c for c in claude_calls if any(op in c for op in status_ops)]
    if set(oracle_status) != set(claude_status):
        diff_info = f"oracle={sorted(oracle_status)}, claude={sorted(claude_status)}"
        if L2 == "unknown_crash" or L2 == "data_structure_misuse":
            detail.append(f"task_status_op_mismatch: {diff_info}")
        else:
            detail.append(f"task_status_op_mismatch: {diff_info}")

    if not detail:
        # Generic body comparison
        if len(claude_body) < len(oracle_body) * 0.3:
            detail.append(f"severely_incomplete: claude={len(claude_body)}chars vs oracle={len(oracle_body)}chars")
            L2 = "incomplete_implementation"
        else:
            detail.append("crash_root_cause_not_heuristically_identified")
            L2 = "needs_manual_inspection"

    return L2, detail


def _classify_logic(claude_body: str, oracle_body: str,
                    call_diff: dict, field_diff: dict,
                    macro_diff: dict, claude_ctrl: dict, oracle_ctrl: dict,
                    sut_function: str, claude_source: str,
                    oracle_source: str, test_output: str) -> tuple:
    """Classify logic deviations (test_failure)."""
    detail = []
    L2 = "logic_deviation"

    # 1. Check for empty/stub implementation
    stripped = claude_body.strip()
    if len(stripped) < 30 or stripped in ("{", "{}", "/* MASKED - implement this function */"):
        empty_patterns = ["(void)spinLock", "(void)", "return OS_OK"]
        if any(p in stripped for p in empty_patterns) or len(stripped) < 50:
            detail.append("stub_implementation: function body is empty or trivial stub")
            return "empty_stub", detail

    # 2. Line-level diff for missing/extra logic blocks
    oracle_lines = oracle_body.splitlines()
    claude_lines = claude_body.splitlines()
    line_diff = simple_diff_lines(oracle_lines, claude_lines)
    removed_lines = [l for op, l in line_diff if op == "-"]
    added_lines = [l for op, l in line_diff if op == "+"]

    # 3. Missing function calls
    oracle_calls_set = set(call_diff.get("shared", [])) | set(call_diff.get("only_oracle", []))
    claude_calls_set = set(call_diff.get("shared", [])) | set(call_diff.get("only_claude", []))
    missing_calls = sorted(oracle_calls_set - claude_calls_set)
    extra_calls = sorted(claude_calls_set - oracle_calls_set)

    if missing_calls:
        detail.append(f"missing_calls: {', '.join(missing_calls[:8])}")

    # 4. Extra/excess calls (hallucinated logic)
    if extra_calls:
        detail.append(f"extra_calls: {', '.join(extra_calls[:8])}")

    # 5. Control flow differences
    for key in ("if_statement", "while_statement", "for_statement",
                "return_statement"):
        o = oracle_ctrl.get(key, 0)
        c = claude_ctrl.get(key, 0)
        if o != c:
            detail.append(f"{key}: oracle={o} claude={c}")

    # 6. Detect specific logic patterns from test output
    # Wrong conditional guard
    if removed_lines:
        # Check if removed lines contain important checks
        for line in removed_lines[:5]:
            stripped_line = line.strip()
            if any(kw in stripped_line for kw in
                   ["if", "NULL", "== 0", "!= 0", "<= ", ">=", "< ", "> ",
                    "return OS_ERROR", "return OS_OK"]):
                detail.append(f"removed_logic: '{stripped_line[:80]}'")
                if L2 == "logic_deviation":
                    L2 = "missing_conditional_guard"

    # 7. Extra guard conditions (overly restrictive)
    extra_conditions = []
    for line in added_lines[:5]:
        stripped_line = line.strip()
        if stripped_line.startswith("if") and any(
                kw in stripped_line for kw in ["NULL", "== 0", "!= 0"]):
            extra_conditions.append(stripped_line[:80])
    if extra_conditions:
        detail.append(f"extra_conditions_added: {'; '.join(extra_conditions)}")
        L2 = "extra_conditional_guard"

    # 8. Missing return/branch paths
    oracle_returns = oracle_ctrl.get("return_statement", 0)
    claude_returns = claude_ctrl.get("return_statement", 0)
    if oracle_returns > claude_returns:
        detail.append(f"fewer_return_statements: oracle={oracle_returns} claude={claude_returns}")
        if L2 == "logic_deviation":
            L2 = "missing_error_path"

    # 9. Check for hardware/spinlock patterns
    if "spinlock" in sut_function.lower() or "spllock" in sut_function.lower():
        detail.append("spinlock_specific: likely HW-register knowledge gap")
        L2 = "hardware_knowledge_gap"

    # 10. Wrong arithmetic / calculation
    if ("<<" in oracle_body or ">>" in oracle_body or "&" in oracle_body or "|" in oracle_body):
        if ("<<" not in claude_body and ">>" not in claude_body and
                "&" not in claude_body and "|" not in claude_body):
            detail.append("missing_bitwise_operations: oracle uses bit ops, Claude doesn't")
            L2 = "wrong_calculation"

    if not detail:
        # Check if implementation is just wrong but structurally similar
        call_jaccard = call_diff.get("jaccard", 0)
        if call_jaccard > 0.7:
            detail.append(f"semantic_drift: call jaccard={call_jaccard:.2f}, structurally similar but behavior differs")
            L2 = "semantic_drift"
        else:
            detail.append("unclassified_logic_deviation")
            L2 = "needs_manual_inspection"

    return L2, detail


# ---------------------------------------------------------------------------
# module extraction
# ---------------------------------------------------------------------------
def classify_module(source_path: str) -> str:
    """Map source_path to a module category."""
    if "signal" in source_path:
        return "ipc/signal"
    if "task" in source_path and "lock" in source_path:
        return "kernel/task_lock"
    if "task" in source_path and "init" in source_path:
        return "kernel/task_init"
    if "task" in source_path and "priority" in source_path:
        return "kernel/task_priority"
    if "task" in source_path and "info" in source_path:
        return "kernel/task_info"
    if "/task" in source_path:
        return "kernel/task"
    if "sched" in source_path:
        return "kernel/sched"
    if "timer" in source_path:
        return "kernel/timer"
    if "spinlock" in source_path:
        return "kernel/spinlock"
    if "irq" in source_path:
        return "kernel/irq"
    if "sys" in source_path:
        return "kernel/sys"
    if "sem" in source_path:
        return "ipc/sem"
    if "queue" in source_path:
        return "ipc/queue"
    if "rwlock" in source_path:
        return "ipc/rwlock"
    if "perf_output" in source_path:
        return "perf/output"
    if "perf_ringbuf" in source_path:
        return "perf/ringbuf"
    if "pmu" in source_path:
        return "perf/pmu"
    if "prt_perf" in source_path:
        return "perf/core"
    if "log" in source_path:
        return "utility/log"
    if "notifier" in source_path:
        return "utility/notifier"
    if "dlmodule" in source_path:
        return "extend/dlmodule"
    return "other"


# ---------------------------------------------------------------------------
# main analysis
# ---------------------------------------------------------------------------
def main():
    # ---- load failed tasks ----
    with open(RESULTS_FILE, encoding="utf-8") as f:
        results = [json.loads(l) for l in f if l.strip()]

    failed = [r for r in results if not r["passed"]]
    print(f"Found {len(failed)} failed tasks\n")

    analysis_results = []
    L1_counter = Counter()
    L2_counter = Counter()
    module_L1 = defaultdict(Counter)
    module_L2 = defaultdict(Counter)
    # Cross matrix: module x (L1+L2)
    cross_matrix = defaultdict(lambda: defaultdict(list))

    for r in failed:
        tid = r["task_id"]
        error_cat = r.get("error_category", "unknown")
        task = r.get("snapshot", {}).get("task", {})
        source_path = task.get("source_path", "?")
        sut_function = task.get("sut_function", "?")
        module = classify_module(source_path)

        traj_file = TRAJECTORY_DIR / f"task_{tid}.log"
        oracle_file = ORACLES_DIR / f"{tid}.c"

        # ---- read trajectory ----
        traj_text = ""
        if traj_file.exists():
            traj_text = traj_file.read_text(encoding="utf-8")

        # extract Claude's injected body
        claude_body = ""
        test_output = ""
        if "=== Injected Body ===" in traj_text:
            parts = traj_text.split("=== Injected Body ===")
            test_split = parts[1].split("=== Test Output ===")
            claude_body = test_split[0].strip()
            if len(test_split) > 1:
                test_output = test_split[1].strip()

        # ---- read oracle ----
        oracle_full = ""
        if oracle_file.exists():
            oracle_full = oracle_file.read_text(encoding="utf-8")

        oracle_body = extract_body(oracle_full, sut_function) if oracle_full else ""

        # ---- detect false negative: test actually passed but framework missed it ----
        actual_pass = False
        pass_rate_match = re.search(r'Pass-Rate:\s*([\d.]+)%', test_output)
        if pass_rate_match and float(pass_rate_match.group(1)) == 100.0:
            actual_pass = True
        # Alternative format: "Test completed." with 0 FAILs
        if not actual_pass and 'Test completed.' in test_output:
            fails = len(re.findall(r'Result\s*:\s*FAIL', test_output))
            if fails == 0:
                actual_pass = True

        is_false_failure = (actual_pass and error_cat in ("crash", "test_failure"))

        # ---- classify ----
        if is_false_failure:
            reason = "crash regex" if error_cat == "crash" else "Pass-Rate format"
            classification = {
                "L1": "false_failure_detection",
                "L2": f"harness_bug_{reason.replace(' ', '_')}",
                "L3_detail": [
                    f"Test actually PASSED but framework classified as {error_cat}. "
                    f"Reason: {reason}. Implementation is correct."
                ],
                "call_diff": {},
                "field_diff": {},
                "macro_diff": {},
                "claude_control": {},
                "oracle_control": {},
            }
        else:
            classification = classify_failure(
                error_cat,
                claude_body,
                oracle_body,
                sut_function,
                source_path,
                claude_body,
                oracle_body or "",
                test_output,
            )

        # ---- accumulate stats ----
        L1 = classification["L1"]
        L2 = f"{L1}/{classification['L2']}"
        L1_counter[L1] += 1
        L2_counter[L2] += 1
        module_L1[module][L1] += 1
        module_L2[module][L2] += 1
        combined = f"{L1}|{classification['L2']}"
        cross_matrix[module][combined].append(tid)

        # ---- per-task record ----
        record = {
            "task_id": tid,
            "sut_function": sut_function,
            "source_path": source_path,
            "module": module,
            "actual_pass": actual_pass,
            "is_false_failure": is_false_failure,
            "classification": classification,
            "body_sizes": {
                "claude_chars": len(claude_body),
                "oracle_chars": len(oracle_body or ""),
                "ratio": (len(claude_body) / len(oracle_body)
                          if oracle_body else 0),
            },
        }
        analysis_results.append(record)

        print(f"[{tid:3s}] {sut_function:35s} | {L1:16s} → {L2:40s}")
        for d in classification["L3_detail"]:
            print(f"       └─ {d[:120]}")
        print()

    # ==================================================================
    # OUTPUT
    # ==================================================================

    # ---- write detailed JSON ----
    OUTPUT_FILE.parent.mkdir(exist_ok=True)
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(analysis_results, f, ensure_ascii=False, indent=2)
    print(f"Detailed report → {OUTPUT_FILE}")

    # ---- summary ----
    print("\n" + "=" * 80)
    print("=== FAILURE CLASSIFICATION SUMMARY ===")
    print("=" * 80)

    print("\n--- L1: Top-level Error Category ---")
    for cat, cnt in L1_counter.most_common():
        print(f"  {cat:25s} {cnt:3d} ({cnt/len(failed)*100:5.1f}%)")

    print(f"\n--- L2: Sub-category (total {len(L2_counter)} types) ---")
    for cat, cnt in L2_counter.most_common():
        print(f"  {cat:45s} {cnt:3d}")

    print("\n--- Module × L1 Cross Table ---")
    all_modules = sorted(module_L1.keys())
    all_l1 = sorted(L1_counter.keys())
    header = f"{'Module':30s}" + "".join(f"{l1:>12s}" for l1 in all_l1) + f"{'Total':>8s}"
    print(header)
    print("-" * len(header))
    for mod in all_modules:
        row = f"{mod:30s}"
        total = 0
        for l1 in all_l1:
            cnt = module_L1[mod].get(l1, 0)
            row += f"{cnt:>12d}"
            total += cnt
        row += f"{total:>8d}"
        print(row)
    # totals row
    row = f"{'TOTAL':30s}"
    for l1 in all_l1:
        row += f"{L1_counter[l1]:>12d}"
    row += f"{len(failed):>8d}"
    print(row)

    print(f"\n--- Module × (L1|L2) Detailed Cross Matrix ---")
    all_combined = sorted(set(
        combo for mod_matrix in cross_matrix.values()
        for combo in mod_matrix.keys()))
    for mod in all_modules:
        entries = cross_matrix.get(mod, {})
        if not entries:
            continue
        print(f"\n  [{mod}] ({sum(len(v) for v in entries.values())} tasks)")
        for combo in sorted(entries.keys(), key=lambda c: -len(entries[c])):
            task_ids = entries[combo]
            print(f"    {combo:50s} {len(task_ids):2d} tasks: {', '.join(task_ids)}")

    # ==================================================================
    # Specific RQ-relevant stats
    # ==================================================================
    print(f"\n{'='*80}")
    print("=== RQ-RELEVANT STATISTICS ===")
    print(f"{'='*80}")

    # RQ1: Which data structures cause most failures?
    all_missing_calls = Counter()
    all_extra_calls = Counter()
    hallucinated_symbols = Counter()

    for rec in analysis_results:
        clf = rec["classification"]
        for detail in clf["L3_detail"]:
            if detail.startswith("missing_calls:"):
                calls = detail.split(": ", 1)[1].split(", ")
                for c in calls:
                    if c:
                        all_missing_calls[c] += 1
            if detail.startswith("extra_calls:"):
                calls = detail.split(": ", 1)[1].split(", ")
                for c in calls:
                    if c:
                        all_extra_calls[c] += 1
            if "undefined_reference:" in detail:
                syms = detail.split("undefined_reference: ", 1)[1].split(", ")
                for s in syms:
                    if s:
                        hallucinated_symbols[s] += 1

    print("\n--- Top 10 Missing API Calls (oracle has, Claude missed) ---")
    for call, cnt in all_missing_calls.most_common(10):
        print(f"  {call:45s} {cnt:2d} failures")

    print("\n--- Top 10 Hallucinated API Symbols (undefined reference) ---")
    for sym, cnt in hallucinated_symbols.most_common(10):
        print(f"  {sym:45s} {cnt:2d} failures")

    # RQ2: Module complexity vs failure rate
    print("\n--- Module Failure Heat Map ---")
    print(f"  {'Module':30s} {'#Failed':>8s} {'Top L2 Failure Mode':45s}")
    print(f"  {'-'*30} {'-'*8} {'-'*45}")
    for mod in sorted(module_L2.keys(),
                      key=lambda x: sum(module_L1[x].values()),
                      reverse=True):
        total = sum(module_L1[mod].values())
        top_l2 = module_L2[mod].most_common(1)[0] if module_L2[mod] else ("?", 0)
        print(f"  {mod:30s} {total:>8d} {top_l2[0]:45s}")

    # ==================================================================
    # Corrected statistics (removing false failure detections)
    # ==================================================================
    false_failure_tasks = [r for r in analysis_results if r["is_false_failure"]]
    genuine_failed = [r for r in analysis_results if not r["is_false_failure"]]
    false_failure_ids = {r["task_id"] for r in false_failure_tasks}

    print(f"\n{'='*80}")
    print("=== CORRECTED STATISTICS (harness bugs removed) ===")
    print(f"{'='*80}")
    print(f"  False failure detections (test actually PASSED): {len(false_failure_tasks)}")
    print(f"    Tasks: {', '.join(sorted(false_failure_ids, key=int))}")
    print(f"  Genuine failures after correction: {len(genuine_failed)}")
    print(f"  Corrected pass rate: {182 + len(false_failure_tasks)}/240 = {(182 + len(false_failure_tasks))/240*100:.1f}%")
    print(f"  (original: 182/240 = 75.8%)")

    # Module-wise correction
    print(f"\n--- Corrected Module Pass Rates ---")
    from collections import defaultdict as dd
    total_per_mod = dd(int)
    passed_per_mod = dd(int)
    with open(RESULTS_FILE, encoding="utf-8") as f:
        all_results = [json.loads(l) for l in f if l.strip()]
    for r in all_results:
        task = r.get("snapshot", {}).get("task", {})
        mod = classify_module(task.get("source_path", ""))
        tid = r["task_id"]
        total_per_mod[mod] += 1
        if r["passed"] or tid in false_failure_ids:
            passed_per_mod[mod] += 1

    print(f"  {'Module':30s} {'Total':>6s} {'Passed':>6s} {'Rate':>8s}")
    print(f"  {'-'*30} {'-'*6} {'-'*6} {'-'*8}")
    for mod in sorted(total_per_mod.keys()):
        t = total_per_mod[mod]
        p = passed_per_mod[mod]
        rate = f"{p/t*100:.1f}%" if t > 0 else "N/A"
        print(f"  {mod:30s} {t:>6d} {p:>6d} {rate:>8s}")

    print("\nDone.")


if __name__ == "__main__":
    main()
