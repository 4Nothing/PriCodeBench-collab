"""
QSem-Claude Agent Pipeline (Docker edition)

对每个 task 的 13 步流程：

  准备阶段 (Steps 1–4):
    1. _prepare_workspace()      → 创建隔离工作区副本 + mask 目标函数 + 隐藏 check_tests
    2. _capture_snapshot()       → 环境快照（model, CLI version, prompt hash, dataset hash）
    3. _snapshot_files()         → 工作区文件 SHA-256 快照（mtime+size 预筛选）
    4. 读取 mask 后的源码

  执行阶段 (Step 5):
    5. _run_claude()             → Docker 沙箱中执行 Claude Code CLI (--network none)

  校验阶段 (Steps 6–9):
    6. _check_file_integrity()   → 对比前后文件快照，检测非目标文件篡改
    7. 读取 Claude 编辑后的源码
    8. 生成 unified diff
    9. check_ast_integrity()     → 归一化等价性检查 + 关键字检查

  测试阶段 (Steps 10–11):
   10. 提取函数体 → 写入 func_under_test.c
   11. _verify_tests()           → 独立 Docker 容器编译并运行测试

  收尾阶段 (Steps 12–13):
   12. _record_trajectory()      → 写轨迹日志
   13. _make_result() + cleanup  → 存结果 + 删除工作区

Claude 是纯黑盒：不负责测试，只负责阅读代码并编辑目标文件。
所有验证由 Python 框架完成。
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
from logger import append_trajectory

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


def strip_ansi(text: str) -> str:
    return _ANSI_RE.sub('', text)


def _docker_user_args():
    """生成 --user 参数列表，使容器内文件写权限与宿主机一致。

    Windows (Docker Desktop + WSL2) 上 os.getuid 不存在，
    此时不传 --user，容器以 root 运行，权限由 Docker Desktop 的
    WSL2 集成层处理。
    """
    try:
        return ["--user", f"{os.getuid()}:{os.getgid()}"]
    except AttributeError:
        return []


class QSemAgent:

    def __init__(self, repo_root, trajectory_dir=None, rag_store=None, rag_embedder=None):
        self.repo_root = Path(repo_root)
        self.trajectory_dir = Path(trajectory_dir) if trajectory_dir else TRAJECTORY_DIR
        self.rag_store = rag_store
        self.rag_embedder = rag_embedder

    # ==================================================================
    # 主入口
    # ==================================================================

    def run(self, task):
        task_id = task["task_id"]
        source_path = task["source_path"]
        sut_function = task["sut_function"]

        start = time.time()
        workspace_dir = None
        snapshot = None

        try:
            # ---- 1. 工作区准备 ----
            workspace_dir = self._prepare_workspace(task)
            repo_dir = workspace_dir / "repo"

            # ---- 2. 环境快照 ----
            snapshot = self._capture_snapshot(task)

            # ---- 3. 文件系统快照 + 读取 mask 源码 ----
            fs_before = self._snapshot_files(repo_dir)
            ws_file = repo_dir / source_path
            masked_source = ws_file.read_text()

            # ---- 5. Docker 沙箱中执行 Claude ----
            claude_completed, claude_output = self._run_claude(task, workspace_dir)
            watchdog_kill = "[WATCHDOG]" in (claude_output or "")

            # ---- 6. 文件完整性检查 ----
            fs_after = self._snapshot_files(repo_dir, previous=fs_before)
            file_tampering = self._check_file_integrity(
                fs_before, fs_after, source_path,
            )

            # ---- 7. 读取 Claude 编辑后的源码 ----
            final_source = ws_file.read_text()

            # ---- 8. 生成 unified diff ----
            diff = "".join(difflib.unified_diff(
                masked_source.splitlines(keepends=True),
                final_source.splitlines(keepends=True),
                fromfile=source_path,
                tofile=source_path,
            ))

            # ---- 9. 归一化等价性检查 ----
            integrity_ok = check_ast_integrity(
                masked_source, final_source, sut_function,
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

            # check_tests 被 restore 后仍是锁定的（555），先解锁
            subprocess.run(
                ["chmod", "-R", "u+w", str(repo_dir / "check_tests")],
                capture_output=True,
            )

            # ---- 10. 提取函数体 → 写入 func_under_test.c ----
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

            func_under_test = (
                repo_dir / "check_tests" / "tasks"
                / f"task_{int(task_id):03d}" / "func_under_test.c"
            )
            self._write_func_body(func_under_test, body)

            # ---- 11. Docker 容器中编译运行测试 ----
            tests_passed, test_output, total_failures, test_error_cat = (
                self._verify_tests(task, workspace_dir)
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
            # ---- 13. 清理工作区 ----
            if workspace_dir and workspace_dir.exists():
                subprocess.run(
                    ["chmod", "-R", "u+w", str(workspace_dir)],
                    capture_output=True,
                )
                shutil.rmtree(workspace_dir, ignore_errors=True)

    # ==================================================================
    # 工作区准备
    # ==================================================================

    def _prepare_workspace(self, task):
        """创建隔离的工作区副本。

        目录结构:
          /tmp/qsem-workspaces/task_N/
            repo/              ← QSemOS 源码副本
              src/             ← 内核源码（Claude 上下文 + 编辑目标）
              check_tests/     ← 测试框架（会被 mv 隐藏，验证时恢复）
              ...
            claude_home/       ← Claude 配置目录

        权限策略:
          - 所有目录 chmod 555 → 不可创建/删除/重命名
          - 所有文件 chmod a-w → 不可修改
          - 目标文件 chmod 644 → 唯一可写的文件
        """
        task_id = task["task_id"]
        ws_dir = WORKSPACE_BASE / f"task_{task_id}"
        repo_dir = ws_dir / "repo"
        claude_home = ws_dir / "claude_home"

        # 清理遗留工作区
        if ws_dir.exists():
            subprocess.run(
                ["chmod", "-R", "u+w", str(ws_dir)],
                capture_output=True,
            )
            shutil.rmtree(ws_dir, ignore_errors=True)

        repo_dir.mkdir(parents=True, exist_ok=True)
        claude_home.mkdir(parents=True, exist_ok=True)

        # 复制 QSemOS 源码（排除 .git）
        try:
            shutil.copytree(
                self.repo_root, repo_dir,
                ignore=shutil.ignore_patterns('.git'),
                dirs_exist_ok=True,
            )
        except (OSError, shutil.Error) as e:
            raise RuntimeError(f"Failed to copy repo to workspace: {e}")

        # 显式验证 .git 未被复制
        git_dir = repo_dir / ".git"
        if git_dir.exists():
            shutil.rmtree(git_dir)

        # 删除预编译的 test 二进制（每个 task 目录下可能有）
        for test_bin in repo_dir.glob("check_tests/tasks/*/test"):
            test_bin.unlink(missing_ok=True)
        for test_bin in repo_dir.glob("check_tests/tasks/*/test_*"):
            if test_bin.is_file() and not test_bin.suffix:
                test_bin.unlink(missing_ok=True)

        # ---- tree-sitter mask ----
        ws_file = repo_dir / task["source_path"]
        apply_mask(ws_file, task["sut_function"], task["masked_source"])

        # ---- 复制 Claude settings + 注入 hook ----
        if CLAUDE_SETTINGS_SRC.exists():
            settings = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            settings.setdefault("hooks", {})["claude-on-completion"] = (
                f"echo '{HOOK_MARKER}'"
            )
            (claude_home / "settings.json").write_text(
                json.dumps(settings, indent=2, ensure_ascii=False)
            )

        # ---- 基础权限（所有文件可读） ----
        subprocess.run(
            ["chmod", "-R", "a+rX", str(repo_dir)],
            capture_output=True,
            check=True,
        )
        subprocess.run(
            ["chmod", "-R", "a+rwX", str(claude_home)],
            capture_output=True,
            check=True,
        )

        # ---- 工作区锁定 ----
        # 子目录 555 = 不可创建/删除/重命名文件
        # repo_dir 自身保持 755，以便 _run_claude 里 mv check_tests
        subprocess.run(
            ["find", str(repo_dir), "-mindepth", "1", "-type", "d",
             "-exec", "chmod", "555", "{}", "+"],
            capture_output=True,
            check=True,
        )
        # 所有文件去写权限
        subprocess.run(
            ["find", str(repo_dir), "-type", "f",
             "-exec", "chmod", "a-w", "{}", "+"],
            capture_output=True,
            check=True,
        )
        # 目标文件设为可写
        target_file = repo_dir / task["source_path"]
        target_file.chmod(0o644)

        return ws_dir

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

        # Docker 镜像 ID
        r = subprocess.run(
            ["docker", "inspect", "--format={{.Id}}", DOCKER_IMAGE],
            capture_output=True, text=True,
        )
        snap["docker_image_id"] = r.stdout.strip() if r.returncode == 0 else None

        # Docker 镜像 digest
        r = subprocess.run(
            ["docker", "inspect", "--format={{index .RepoDigests 0}}",
             DOCKER_IMAGE],
            capture_output=True, text=True,
        )
        snap["docker_image_digest"] = r.stdout.strip() if r.returncode == 0 else None

        # Claude Code CLI 版本
        r = subprocess.run(
            ["docker", "run", "--rm", "--entrypoint", "claude",
             DOCKER_IMAGE, "--version"],
            capture_output=True, text=True,
        )
        snap["claude_code_version"] = r.stdout.strip() if r.returncode == 0 else None

        # Prompt hash
        prompt = self._build_prompt(task)
        snap["prompt_sha256"] = hashlib.sha256(prompt.encode()).hexdigest()
        snap["prompt_text"] = prompt

        snap["task"] = task

        if DATASET_PATH.exists():
            snap["dataset_sha256"] = hashlib.sha256(
                DATASET_PATH.read_bytes()
            ).hexdigest()

        snap["python_version"] = sys.version

        return snap

    # ==================================================================
    # 文件系统快照
    # ==================================================================

    def _snapshot_files(self, repo_dir, previous=None):
        """扫描工作区文件，返回 {相对路径: (sha256, mtime, size)}。

        用 SHA256 做指纹。当传入 previous 时，对 mtime+size 均未变的文件
        复用旧 SHA256，避免重复哈希。
        """
        result = subprocess.run(
            ["find", str(repo_dir), "-type", "f",
             "-printf", "%P\\0%s\\0%T@\\0"],
            capture_output=True, text=True,
        )
        prev = previous or {}
        snap = {}
        tokens = result.stdout.split("\0")
        for i in range(0, len(tokens) - 2, 3):
            relpath = tokens[i]
            size_str = tokens[i + 1]
            mtime_str = tokens[i + 2]
            if not relpath or not size_str or not mtime_str:
                continue
            mtime = float(mtime_str)
            size = int(size_str)

            if relpath in prev:
                _, prev_mtime, prev_size = prev[relpath]
                if mtime == prev_mtime and size == prev_size:
                    snap[relpath] = (prev[relpath][0], mtime, size)
                    continue

            file_path = repo_dir / relpath
            h = hashlib.sha256(file_path.read_bytes()).hexdigest()
            snap[relpath] = (h, mtime, size)
        return snap

    def _check_file_integrity(self, before, after, target_path):
        """比较前后文件快照，检测目标文件之外的变更。

        Returns:
            list[str]: 被修改/创建/删除的文件描述，空列表 = 无篡改
        """
        modified = []
        for path, (after_hash, after_mtime, after_size) in after.items():
            if path == target_path:
                continue
            if path not in before:
                modified.append(f"{path} (new)")
            else:
                before_hash, before_mtime, before_size = before[path]
                if before_mtime == after_mtime and before_size == after_size:
                    continue
                if before_hash != after_hash:
                    modified.append(f"{path} (modified)")
        for path in before:
            if path != target_path and path not in after:
                modified.append(f"{path} (deleted)")
        return modified

    # ==================================================================
    # Docker Claude 执行
    # ==================================================================

    @staticmethod
    def _claude_env_args():
        """从 settings.json 读取 Claude API 环境变量，生成 Docker -e 参数。"""
        try:
            cfg = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            env = cfg.get("env", {})
            allowed = ALLOWED_CLAUDE_ENV
            args = []
            for k, v in env.items():
                if k in allowed:
                    args.extend(["-e", f"{k}={v}"])
            return args
        except Exception:
            return []

    def _run_claude(self, task, workspace_dir):
        """在 Docker 容器中执行 Claude Code CLI。

        隔离措施：
          - --network none（彻底断网）
          - --cap-drop ALL, --security-opt no-new-privileges
          - --pids-limit, --memory, --cpus 限制
          - --user 匹配宿主机 UID/GID

        check_tests/ 在启动前 mv 隐藏，finally 中恢复。
        """
        prompt = self._build_prompt(task)
        repo_dir = workspace_dir / "repo"
        claude_home = workspace_dir / "claude_home"
        rng = f"{os.getpid()}_{id(task)}"

        # 隐藏 check_tests —— mv 到随机名，模型无法猜测路径
        # （repo_dir 保持 755，子目录均为 555，mv 操作合法）
        ct_dir = repo_dir / "check_tests"
        ct_hidden = None
        if ct_dir.exists():
            ct_hidden = repo_dir / f".check_tests_hidden_{rng}"
            shutil.move(str(ct_dir), str(ct_hidden))

        try:
            docker_args = [
                "docker", "run", "--rm", "-i",
                "--init",
                "--network", CLAUDE_NETWORK_MODE,
                "--cap-drop", "ALL",
                "--security-opt", "no-new-privileges",
                "--pids-limit", DOCKER_PIDS_LIMIT,
                "--memory", DOCKER_MEMORY,
                "--cpus", DOCKER_CPUS,
                *_docker_user_args(),
                *self._claude_env_args(),
                "-e", "BUILD_DIR=/tmp/build",
                "-e", "TERM=dumb",
                "-e", "NO_COLOR=1",
                "-e", "CI=1",
                "-e", "PAGER=cat",
                "-v", f"{repo_dir}:/workspace:rw",
                "-v", f"{claude_home}:/home/agent/.claude:rw",
                "--tmpfs", "/tmp:size=1g,exec",
                "--tmpfs", "/home/agent/.cache:size=500m",
                "--tmpfs", "/home/agent/.config:size=100m",
                "-w", "/workspace",
                "--entrypoint", "bash",
                DOCKER_IMAGE,
                "-c", 'claude --dangerously-skip-permissions -p "$(cat)" 2>&1',
            ]

            print(f"  ┌─ {'='*60}")
            print(f"  │ Docker agent: {task['sut_function']} (task {task['task_id']})")
            print(f"  │ source: {task['source_path']}")
            print(f"  │ workspace: {repo_dir}")
            print(f"  └─ {'='*60}")

            proc = subprocess.Popen(
                docker_args,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
            )

            proc.stdin.write(prompt)
            proc.stdin.close()

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

            # 排空残留输出
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
                print(f"\n  │ Claude completed (hook or clean exit)")
            else:
                print(f"\n  │ Claude did not report completion (rc={proc.returncode})")

            return claude_completed, full_output

        finally:
            # 恢复 check_tests
            if ct_hidden is not None and ct_hidden.exists():
                try:
                    shutil.move(str(ct_hidden), str(ct_dir))
                except Exception:
                    pass

    # ==================================================================
    # 函数体写入 func_under_test.c
    # ==================================================================

    def _write_func_body(self, path, body):
        """用 tree-sitter 定位第一个函数定义的 body 并替换。"""
        from tree_sitter import Language, Parser
        import tree_sitter_c
        _C_LANG = Language(tree_sitter_c.language())
        parser = Parser(_C_LANG)

        source = path.read_text()
        tree = parser.parse(bytes(source, 'utf8'))
        root = tree.root_node

        for child in root.children:
            if child.type == 'function_definition':
                fn_body = child.child_by_field_name('body')
                if fn_body is not None:
                    bs = source.encode('utf-8')
                    body_start = len(
                        bs[:fn_body.start_byte + 1].decode('utf-8', 'replace')
                    )
                    body_end = len(
                        bs[:fn_body.end_byte - 1].decode('utf-8', 'replace')
                    )
                    new_source = source[:body_start] + body + source[body_end:]
                    path.write_text(new_source)
                    return

        raise RuntimeError(f"No function definition found in {path}")

    # ==================================================================
    # 测试验证
    # ==================================================================

    def _verify_tests(self, task, workspace_dir):
        """在独立 Docker 容器中编译并运行单元测试。

        验证容器与 Claude 容器分离：
          - --network none（完全无网络）
          - 只读挂载 check_tests/include/

        判定逻辑：
          PASS = 编译成功 AND 无崩溃 AND Pass-Rate: 100.00%
        """
        task_id = task["task_id"]
        repo_dir = workspace_dir / "repo"
        task_subdir = f"check_tests/tasks/task_{int(task_id):03d}"
        cmd = f"cd /workspace/{task_subdir} && make && ./test"

        docker_args = [
            "docker", "run", "--rm", "-i",
            "--network", "none",
            "--cap-drop", "ALL",
            "--security-opt", "no-new-privileges",
            "--pids-limit", DOCKER_PIDS_LIMIT,
            "--memory", DOCKER_MEMORY,
            "--cpus", DOCKER_CPUS,
            *_docker_user_args(),
            "-v", f"{repo_dir}:/workspace:rw",
            "--tmpfs", "/tmp:size=1g,exec",
            "-w", "/workspace",
            "--entrypoint", "bash",
            DOCKER_IMAGE,
            "-c", (
                # 解锁目录权限以允许 make 创建 .o 文件和 test 二进制
                f"find /workspace -type d -exec chmod 755 {{}} + 2>/dev/null; "
                f"find /workspace -type f -exec chmod u+w {{}} + 2>/dev/null; "
                f"{cmd}"
            ),
        ]

        try:
            p = subprocess.run(
                docker_args,
                capture_output=True, text=True,
                timeout=TEST_TIMEOUT,
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

            # 运行崩溃（只在 stdout 检测，避免编译 warning 误匹配）
            crash = bool(
                re.search(
                    r"(?:Segmentation\s+fault|Aborted|\bpanic\b)",
                    stdout,
                )
            )

            # shell 错误
            shell_fail = bool(re.search(
                r'(?:No such file or directory|'
                r'command not found|cannot execute|cannot open)',
                stderr,
            ))

            # 通过的标志: Pass-Rate: 100.00%
            passed = bool(re.search(
                r'Pass-Rate:\s*100\.00%',
                stdout,
            ))
            # 备选格式: "Test completed." 且无 FAIL
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
    # Prompt 构建
    # ==================================================================

    def _build_prompt(self, task):
        prompt = TASK_PROMPT.format(
            sut_function=task["sut_function"],
            source_path=task["source_path"],
        )
        if self.rag_store and self.rag_embedder:
            from rag.retrieve import retrieve
            rag_context = retrieve(task["sut_function"], task["source_path"],
                                   self.rag_store, self.rag_embedder)
            if rag_context:
                prompt = rag_context + chr(10) + chr(10) + prompt
        return prompt

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
