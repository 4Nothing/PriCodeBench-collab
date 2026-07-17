"""
QSem-Claude Agent Pipeline

对每个 task:
  1. 备份源文件和 func_under_test.c
  2. 锁定 src/ 权限（只有目标文件可写）
  3. mask 目标函数体
  4. 环境快照
  5. 隐藏 check_tests，启动 Claude
  6. 恢复 check_tests 和权限
  7. 归一化等价性检查 + 文件篡改检测
  8. 提取函数体，写入 func_under_test.c
  9. 编译运行测试
 10. 记录结果
 11. 恢复文件
"""
import difflib
import hashlib
import json
import os
import re
import selectors
import shutil
import subprocess
import sys
import time
from pathlib import Path

from config import *
from prompt import TASK_PROMPT
from mask import (
    apply_mask,
    check_ast_integrity,
    extract_function_body,
)
from mask import _find_function, _char_offset
from logger import append_trajectory
from tree_sitter import Language, Parser
import tree_sitter_c

ERROR_NONE = "none"
ERROR_ILLEGAL = "illegal_modifications"
ERROR_TAMPERING = "file_tampering"
ERROR_COMPILE = "compile_error"
ERROR_CRASH = "crash"
ERROR_TEST_FAIL = "test_failure"
ERROR_TEST_NOT_RUN = "test_not_executed"
ERROR_TIMEOUT = "timeout"
ERROR_WATCHDOG = "watchdog"
ERROR_SETUP = "workspace_setup"
ERROR_EXCEPTION = "exception"
ERROR_API = "api_error"

_ANSI_RE = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

HOOK_MARKER = "=== QSEM_TASK_COMPLETED ==="

_C_LANG = Language(tree_sitter_c.language())


def strip_ansi(text: str) -> str:
    return _ANSI_RE.sub('', text)


def _tree_parser():
    p = Parser(_C_LANG)
    return p


class QSemAgent:

    def __init__(self, repo_root, trajectory_dir=None):
        self.repo_root = Path(repo_root)
        self.trajectory_dir = Path(trajectory_dir) if trajectory_dir else SCRIPT_DIR / "trajectory"

    # ==================================================================
    # 主入口
    # ==================================================================

    def run(self, task):
        task_id = task["task_id"]
        source_path = self.repo_root / task["source_path"]
        sut_function = task["sut_function"]
        task_dir = TASKS_DIR / f"task_{int(task_id):03d}"
        func_under_test = task_dir / "func_under_test.c"

        start = time.time()
        snapshot = None
        backup_source = None
        backup_func = None
        src_dir = self.repo_root / "src"

        try:
            # ---- 1. 备份 ----
            backup_source = source_path.read_text()
            backup_func = func_under_test.read_text()

            # ---- 2. 锁定 src/ 权限 ----
            self._lock_src(src_dir, source_path)

            # ---- 3. mask 目标函数体 ----
            apply_mask(source_path, sut_function, task["masked_source"])
            masked_source = source_path.read_text()

            # ---- 4. 环境快照 (含 src/ 文件哈希，用于后续篡改检测) ----
            snapshot = self._capture_snapshot(task)
            src_hashes_before = self._hash_src_files(src_dir, source_path)

            # ---- 5. 启动 Claude ----
            claude_completed, claude_output = self._run_claude(task)

            # ---- 6. 恢复 check_tests (在 _run_claude 的 finally 中) ----
            # _run_claude 返回前已恢复 check_tests
            watchdog_kill = "[WATCHDOG]" in (claude_output or "")

            # ---- 7. 归一化等价性检查 + 文件篡改检测 ----
            final_source = source_path.read_text()

            diff = "".join(difflib.unified_diff(
                masked_source.splitlines(keepends=True),
                final_source.splitlines(keepends=True),
                fromfile=task["source_path"],
                tofile=task["source_path"],
            ))

            integrity_ok = check_ast_integrity(
                masked_source, final_source, sut_function,
            )
            file_tampering = self._check_other_files(
                src_dir, source_path, src_hashes_before,
            )

            illegal = []
            if not integrity_ok:
                illegal.append(
                    "integrity violation: code outside target function changed"
                )
            if file_tampering:
                illegal.append(
                    f"file tampering detected: {'; '.join(file_tampering)}"
                )

            self._record_trajectory(task, claude_output, diff, illegal)

            # 恢复 src/ 权限（完整性检查已完成）
            self._unlock_src(src_dir)

            # ---- 8. 写入 func_under_test.c ----
            body = extract_function_body(final_source, sut_function)
            if body is None:
                return self._make_result(
                    task, start,
                    passed=False,
                    claude_completed=claude_completed,
                    illegal=illegal,
                    error="could not extract function body from agent output",
                    error_category=ERROR_EXCEPTION,
                    diff=diff,
                    snapshot=snapshot,
                )

            self._write_func_body(func_under_test, body)

            # ---- 9. 运行测试 ----
            tests_passed, test_output, total_failures, test_error_cat = (
                self._verify_tests(task)
            )

            append_trajectory(
                self.trajectory_dir / f"task_{task_id}.log",
                f"\n{'='*60}\n"
                f"=== Injected Body ===\n"
                f"{'='*60}\n"
                f"{body}\n"
                f"\n{'='*60}\n"
                f"=== Test Output ===\n"
                f"{'='*60}\n"
                f"{test_output}\n"
            )

            test_error = None
            test_cat = test_error_cat
            if watchdog_kill:
                test_error = "watchdog_killed_claude"
                test_cat = test_cat or ERROR_WATCHDOG
            elif not claude_completed and tests_passed:
                test_error = "claude_did_not_complete"
                test_cat = test_cat or ERROR_API
            elif not tests_passed:
                test_error = test_error_cat or "test_failure"

            return self._make_result(
                task, start,
                passed=tests_passed,
                claude_completed=claude_completed,
                illegal=illegal,
                error=test_error,
                error_category=test_cat,
                diff=diff,
                snapshot=snapshot,
                total_test_failures=total_failures,
            )

        except Exception as e:
            return self._make_result(
                task, start,
                passed=False,
                claude_completed=False,
                illegal=[],
                error=str(e),
                error_category=ERROR_EXCEPTION,
                snapshot=snapshot,
            )

        finally:
            # ---- 恢复文件 ----
            self._unlock_src(src_dir)
            if backup_source is not None:
                try:
                    source_path.write_text(backup_source)
                except Exception:
                    pass
            if backup_func is not None:
                try:
                    func_under_test.write_text(backup_func)
                except Exception:
                    pass

    # ==================================================================
    # 权限控制
    # ==================================================================

    def _lock_src(self, src_dir, target_file):
        """锁定 src/ 目录：目录 555，文件 a-w，目标文件 u+w。"""
        src_dir.chmod(0o555)
        for d in src_dir.rglob("*"):
            if d.is_dir():
                d.chmod(0o555)
        for f in src_dir.rglob("*"):
            if f.is_file():
                f.chmod(0o444)
        target_file.chmod(0o644)

    def _unlock_src(self, src_dir):
        """恢复 src/ 目录权限：目录 755，文件 u+w。"""
        src_dir.chmod(0o755)
        for d in src_dir.rglob("*"):
            if d.is_dir():
                d.chmod(0o755)
        for f in src_dir.rglob("*"):
            if f.is_file():
                f.chmod(0o644)

    # ==================================================================
    # 环境快照
    # ==================================================================

    def _capture_snapshot(self, task):
        snap = {}

        snap["timestamp"] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())

        try:
            cfg = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            snap["claude_model"] = cfg.get("model", "unknown")
        except Exception:
            snap["claude_model"] = "unknown"

        r = subprocess.run(
            ["claude", "--version"],
            capture_output=True, text=True,
        )
        snap["claude_code_version"] = r.stdout.strip() if r.returncode == 0 else None

        prompt = self._build_prompt(task)
        snap["prompt_sha256"] = hashlib.sha256(prompt.encode()).hexdigest()
        snap["prompt_text"] = prompt

        snap["task"] = task

        if DATASET_PATH.exists():
            snap["dataset_sha256"] = hashlib.sha256(
                DATASET_PATH.read_bytes()
            ).hexdigest()

        snap["tree_sitter_c_version"] = getattr(tree_sitter_c, '__version__', 'unknown')
        snap["python_version"] = sys.version

        return snap

    # ==================================================================
    # Claude CLI 调用
    # ==================================================================

    def _run_claude(self, task):
        """直接调用 Claude Code CLI 编辑 QSemOS 源码。

        隐藏 check_tests (mv)，用 chmod 000 禁止访问 oracles、数据集、
        results、trajectory、qsem-claude 自身。prompt 通过 stdin 传入。
        finally 块恢复所有隐藏项。
        """
        prompt = self._build_prompt(task)
        rng = f"{os.getpid()}_{id(task)}"

        moves = []        # (hidden_path, original_path) for mv restore
        chmods = []       # (path, original_mode) for chmod restore

        # check_tests — mv 隐藏（同名猜测风险低，目录深，mv 更可靠）
        ct_dir = self.repo_root / "check_tests"
        if ct_dir.exists():
            dst = self.repo_root / f".check_tests_hidden_{rng}"
            shutil.move(str(ct_dir), str(dst))
            moves.append((dst, ct_dir))

        # 敏感路径 — chmod 000 禁止访问
        sensitive = [
            ORACLES_DIR,
            DATASET_PATH,
            RESULTS_DIR,
            self.trajectory_dir,
            SCRIPT_DIR,   # qsem-claude/ 自身（含 README 等）
        ]
        for path in sensitive:
            if path.exists():
                chmods.append((path, path.stat().st_mode))
                path.chmod(0o000)

        try:
            claude_args = [
                "claude",
                "--dangerously-skip-permissions",
                "--disallowed-tools", "WebSearch,WebFetch",
                "-p", prompt,
                "--output-format", "text",
            ]

            print(f"  ┌─ {'='*60}")
            print(f"  │ Claude agent: {task['sut_function']} (task {task['task_id']})")
            print(f"  │ source: {task['source_path']}")
            print(f"  └─ {'='*60}")

            proc = subprocess.Popen(
                claude_args,
                stdin=subprocess.DEVNULL,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                cwd=str(self.repo_root),
            )

            sel = selectors.DefaultSelector()
            sel.register(proc.stdout, selectors.EVENT_READ)

            output_chunks = []
            last_activity = time.time()
            claude_completed = False

            while True:
                if proc.poll() is not None:
                    break

                idle = time.time() - last_activity
                if idle > WATCHDOG_TIMEOUT:
                    proc.kill()
                    output_chunks.append(
                        "\n[WATCHDOG] Task idle for "
                        f"{WATCHDOG_TIMEOUT}s with no output — killed.\n"
                    )
                    break

                events = sel.select(timeout=1.0)
                for key, _ in events:
                    line = key.fileobj.readline()
                    if not line:
                        break
                    output_chunks.append(line)
                    last_activity = time.time()
                    clean = strip_ansi(line)
                    sys.stdout.write(clean)
                    sys.stdout.flush()

                    if HOOK_MARKER in clean.strip():
                        claude_completed = True

            sel.unregister(proc.stdout)
            sel.close()

            while True:
                try:
                    line = proc.stdout.readline()
                except Exception:
                    break
                if not line:
                    break
                output_chunks.append(line)
                clean = strip_ansi(line)
                sys.stdout.write(clean)
                sys.stdout.flush()
                if HOOK_MARKER in clean.strip():
                    claude_completed = True

            proc.wait(timeout=10)
            full_output = "".join(output_chunks)

            if not claude_completed and proc.returncode == 0:
                claude_completed = True

            if claude_completed:
                print(f"\n  │ Claude completed")
            else:
                print(f"\n  │ Claude did not report completion (rc={proc.returncode})")

            return claude_completed, full_output

        finally:
            for src, dst in reversed(moves):
                try:
                    if src.exists():
                        shutil.move(str(src), str(dst))
                except Exception:
                    pass
            for path, mode in reversed(chmods):
                try:
                    if path.exists():
                        path.chmod(mode)
                except Exception:
                    pass

    # ==================================================================
    # 函数体写入 func_under_test.c
    # ==================================================================

    def _write_func_body(self, path: Path, body: str):
        """用 tree-sitter 定位第一个函数定义的 body 并替换。"""
        source = path.read_text()
        parser = _tree_parser()
        tree = parser.parse(bytes(source, 'utf8'))
        root = tree.root_node

        for child in root.children:
            if child.type == 'function_definition':
                fn_body = child.child_by_field_name('body')
                if fn_body is not None:
                    bs = source.encode('utf-8')
                    body_start = len(bs[:fn_body.start_byte + 1].decode('utf-8', 'replace'))
                    body_end = len(bs[:fn_body.end_byte - 1].decode('utf-8', 'replace'))
                    new_source = source[:body_start] + body + source[body_end:]
                    path.write_text(new_source)
                    return

        raise RuntimeError(f"No function definition found in {path}")

    # ==================================================================
    # 测试验证
    # ==================================================================

    def _verify_tests(self, task):
        cmd = task["run_command"]

        try:
            p = subprocess.run(
                cmd,
                shell=True,
                capture_output=True, text=True,
                timeout=TEST_TIMEOUT,
                cwd=str(PRIVATE_CODE),
            )

            stdout = p.stdout or ""
            stderr = p.stderr or ""
            full = stdout + stderr
            test_out = stdout + "\n--- stderr ---\n" + stderr

            # 编译失败
            build_fail = bool(
                re.search(
                    r"(?:undefined reference to|"
                    r"collect2:\s*error|"
                    r"compilation terminated\.|"
                    r"make(?:\[\d+\])?:\s*\*\*\*|"
                    r"error:|fatal error:)",
                    full,
                    re.IGNORECASE,
                )
            )

            # 运行崩溃
            # 运行崩溃 — 只在 stdout（运行时输出）中检测，避免编译 warning 误匹配
            crash = bool(
                re.search(
                    r"(?:Segmentation\s+fault|Aborted|\bpanic\b|signal\s+\d+)",
                    stdout,
                )
            )

            # shell 错误
            shell_fail = bool(re.search(
                r'(?:No such file or directory|'
                r'command not found|cannot execute|cannot open)',
                stderr,
            ))

            # 通过的标志: Pass-Rate: 100.00% 或 Test completed. + 0 FAILs
            passed = bool(re.search(
                r'Pass-Rate:\s*100\.00%',
                stdout,
            ))
            # 备选通过格式: "Test completed." 且无 FAIL
            if not passed and 'Test completed.' in stdout:
                fails = len(re.findall(r'Result\s*:\s*FAIL', stdout))
                if fails == 0:
                    passed = True

            total_failures = 0
            error_cat = None
            if build_fail:
                passed = False
                error_cat = ERROR_COMPILE
            elif crash:
                passed = False
                error_cat = ERROR_CRASH
            elif shell_fail:
                passed = False
                error_cat = ERROR_TEST_NOT_RUN
            elif not passed:
                error_cat = ERROR_TEST_FAIL

            return passed, test_out, total_failures, error_cat

        except subprocess.TimeoutExpired:
            return False, "[TIMEOUT] Test command exceeded timeout.", -1, ERROR_TIMEOUT

    # ==================================================================
    # 文件篡改检测
    # ==================================================================

    def _hash_src_files(self, src_dir, target_path):
        """扫描 src/ 所有文件（排除目标文件），返回 {路径: SHA256}。"""
        target_abs = target_path.resolve()
        hashes = {}
        for f in src_dir.rglob("*"):
            if f.is_file() and f.resolve() != target_abs:
                hashes[str(f.resolve())] = hashlib.sha256(
                    f.read_bytes()
                ).hexdigest()
        return hashes

    def _check_other_files(self, src_dir, target_path, hashes_before):
        """对比当前 src/ 文件哈希与 before 快照，检测篡改。"""
        target_abs = target_path.resolve()
        hashes_after = {}
        for f in src_dir.rglob("*"):
            if f.is_file() and f.resolve() != target_abs:
                hashes_after[str(f.resolve())] = hashlib.sha256(
                    f.read_bytes()
                ).hexdigest()

        changed = []
        for path, h in hashes_before.items():
            if path not in hashes_after:
                changed.append(f"{path} (deleted)")
            elif hashes_after[path] != h:
                changed.append(f"{path} (modified)")
        for path in hashes_after:
            if path not in hashes_before:
                changed.append(f"{path} (new)")

        rel_changed = []
        for c in changed:
            try:
                rel = str(Path(c.split(" ")[0]).relative_to(self.repo_root))
            except ValueError:
                rel = c
            rel_changed.append(rel)

        return rel_changed

    # ==================================================================
    # Prompt 构建
    # ==================================================================

    def _build_prompt(self, task):
        return TASK_PROMPT.format(
            sut_function=task["sut_function"],
            source_path=task["source_path"],
        )

    # ==================================================================
    # 日志 / 结果
    # ==================================================================

    def _record_trajectory(self, task, claude_out, diff, illegal):
        traj_file = self.trajectory_dir / f"task_{task['task_id']}.log"

        parts = []

        if claude_out is not None:
            truncated = claude_out
            if len(truncated) > MAX_OUTPUT_LENGTH:
                half = MAX_OUTPUT_LENGTH // 2
                truncated = (
                    truncated[:half]
                    + f"\n[... TRUNCATED {len(truncated) - 2 * half} bytes ...]\n"
                    + truncated[-half:]
                )
            parts.append(
                f"{'='*60}\n"
                f"=== Claude Session ===\n"
                f"{'='*60}\n"
                f"{truncated}\n"
            )

        if diff is not None and diff.strip():
            parts.append(
                f"{'='*60}\n"
                f"=== Diff ===\n"
                f"{'='*60}\n"
                f"{diff}\n"
            )

        if illegal:
            parts.append(
                f"{'='*60}\n"
                f"=== ILLEGAL MODIFICATIONS ===\n"
                f"{'='*60}\n"
                + "\n".join(illegal) + "\n"
            )

        append_trajectory(traj_file, "\n".join(parts))

    def _make_result(
        self, task, start,
        passed, claude_completed, illegal,
        error=None, error_category=None, diff=None,
        file_tampering=None, snapshot=None, total_test_failures=None,
    ):
        elapsed = round(time.time() - start, 1)

        result = {
            "task_id": task["task_id"],
            "passed": passed,
            "claude_completed": claude_completed,
            "runtime_s": elapsed,
            "illegal_changes": illegal,
            "file_tampering": file_tampering or [],
            "error": error,
            "error_category": error_category or ERROR_NONE,
        }

        if diff is not None:
            result["patch"] = diff

        if snapshot is not None:
            result["snapshot"] = snapshot

        if total_test_failures is not None:
            result["total_test_failures"] = total_test_failures

        return result
