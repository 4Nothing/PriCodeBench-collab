"""
ClaudeAgent Pipeline 核心

完整的 13 步 task 执行流程：

  准备阶段 (Steps 1–4):
    1. hard_reset_repo()       → git 仓库重置到干净状态
    2. _prepare_workspace()     → 创建隔离的工作区副本 + mask 目标函数 + 移除 .git
    3. _capture_snapshot()      → 完整环境快照（git HEAD, docker digest, prompt hash, model）
    4. _snapshot_files()        → 扫描工作区文件列表 + 读取 mask 后的源码

  执行阶段 (Step 5):
    5. _run_claude()            → Docker 沙箱中执行 Claude Code CLI

  校验阶段 (Steps 6–9):
    6. 文件完整性检查           → 先对比快照确认改了什么文件，再决定读谁
    7. 读取 Claude 编辑后的源码  → 读取被修改的文件
    8. 生成 unified diff        → 对比 mask 版和最终版
    9. 归一化等价性检查          → 归一化对比（判断非函数体代码语义是否被破坏）
                                   + 关键字检查（防止 #define / typedef 等作弊）

  测试阶段 (Step 10):
   10. _verify_tests()          → 独立断网 Docker 容器编译并运行测试

  收尾阶段 (Steps 11–13):
   11. _record_trajectory()     → 写轨迹日志
   12. _make_result()           → 存结构化结果（含快照数据）
   13. cleanup + verify         → 删除工作区 + 验证清理结果 + git reset

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
from git_utils import hard_reset_repo
from logger import append_trajectory

# 错误分类 —— 供结果分析和统计使用
# 与 error 字符串互补：error 是人读的描述，error_category 是机读的分类
ERROR_NONE = "none"
ERROR_ILLEGAL = "illegal_modifications"    # 归一化等价性检查或关键字检查失败
ERROR_TAMPERING = "file_tampering"          # 非目标文件被修改
ERROR_COMPILE = "compile_error"             # 编译失败
ERROR_CRASH = "crash"                       # 段错误 / panic / abort
ERROR_TEST_FAIL = "test_failure"            # 目标测试失败
ERROR_TEST_NOT_RUN = "test_not_executed"    # 测试二进制未启动
ERROR_TIMEOUT = "timeout"                   # 验证容器超时
ERROR_WATCHDOG = "watchdog"                 # Claude 进程无输出被 kill
ERROR_SETUP = "workspace_setup"             # 工作区准备失败
ERROR_EXCEPTION = "exception"               # 未预期的异常
ERROR_API = "api_error"                     # Claude API 调用失败

# 移除 ANSI 转义序列的正则（Claude 输出可能带颜色控制字符）
_ANSI_RE = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')


def strip_ansi(text: str) -> str:
    """去除字符串中的 ANSI 转义码（颜色/光标控制等）。"""
    return _ANSI_RE.sub('', text)


def _docker_user_args():
    """生成 --user 参数列表，使容器内文件写权限与宿主机一致。

    返回格式: ["--user", "uid:gid"]
    这样容器内创建的文件归宿主用户所有，避免 root 所有的文件无法清理。
    """
    return ["--user", f"{os.getuid()}:{os.getgid()}"]


class ClaudeAgent:

    def __init__(self, repo_root):
        """初始化 Agent。

        Args:
            repo_root: RIOT 仓库根目录的 Path 对象
        """
        self.repo_root = Path(repo_root)

    # ==================================================================
    # 主入口
    # ==================================================================

    def run(self, task, dataset_paths=None):
        """执行完整的 task pipeline。

        Args:
            task: dict，包含:
                - task_id:       唯一标识
                - source_path:   目标源文件路径（相对 RIOT 根目录）
                - sut_function:  待补全的函数名
                - masked_code:   mask 回退用的原始函数签名
            dataset_paths: 可选，数据集 JSONL 文件路径列表，用于快照中记录 hash
                - unit_test:     对应的单元测试名
                - run_command:   验证时的编译+测试命令
        Returns:
            dict: {
                "task_id", "passed", "claude_completed",
                "runtime_s", "illegal_changes",
                "file_tampering", "error", "patch",
                "snapshot", "cleanup_errors"
            }
        """
        file_path = self.repo_root / task["source_path"]
        start = time.time()
        workspace_dir = None
        snapshot = None
        cleanup_errors = []

        try:
            # ---- 步骤 1: git 重置 ----
            hard_reset_repo(self.repo_root, clean=True)

            # ---- 步骤 2: 工作区准备 ----
            workspace_dir = self._prepare_workspace(task)
            repo_dir = workspace_dir / "repo"

            # ---- 步骤 3: 环境快照（用于日后复现） ----
            snapshot = self._capture_snapshot(task, repo_dir, dataset_paths)

            # ---- 步骤 4: 文件系统快照 + 读取 mask 源码 ----
            fs_before = self._snapshot_files(repo_dir)
            ws_file = repo_dir / task["source_path"]
            masked_source = ws_file.read_text()

            # ---- 步骤 5: 执行 Claude ----
            # _run_claude 结尾调用 proc.wait()，保证子进程已完全退出。
            # 因此 Step 6 的快照和 Step 7 的读取之间不存在 TOCTOU——
            # Claude 进程已死，没有进程能在快照后继续写目标文件。
            claude_completed, claude_output = self._run_claude(task, workspace_dir)
            watchdog_kill = "[WATCHDOG]" in (claude_output or "")

            # ---- 步骤 6: 先检查文件完整性，再决定读什么 ----
            fs_after = self._snapshot_files(repo_dir, previous=fs_before)
            file_tampering = self._check_file_integrity(
                fs_before, fs_after, task["source_path"],
            )

            # ---- 步骤 7: 读取 Claude 编辑后的最终源码 ----
            final_source = ws_file.read_text()

            # ---- 步骤 8: 生成 unified diff ----
            diff = "".join(difflib.unified_diff(
                masked_source.splitlines(keepends=True),
                final_source.splitlines(keepends=True),
                fromfile=task["source_path"],
                tofile=task["source_path"],
            ))

            # ---- 步骤 9: 归一化等价性检查 ----
            # 将目标函数体替换为占位符后比较两个版本，
            # 如果不一致 → 说明函数外代码被修改。
            integrity_ok = check_ast_integrity(
                masked_source, final_source, task["sut_function"],
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

            # 记录轨迹日志（含 Claude 输出、diff、违规信息）
            self._record_trajectory(task, claude_output, diff, illegal)

            # ---- 步骤 10: 测试验证 ----
            # 违规不阻断测试，同时记录违规和测试结果
            tests_passed, test_output, total_failures, test_error_cat = (
                self._verify_tests(task, workspace_dir)
            )

            # 将验证输出追加到轨迹日志
            append_trajectory(
                TRAJECTORY_DIR / f"task_{task['task_id']}.log",
                f"\n{'='*60}\n"
                f"=== Harness Verify ===\n"
                f"{'='*60}\n"
                f"{test_output}\n"
            )

            # 若 Claude 未完成（hook 未触发且 returncode != 0），
            # 但测试通过了，仍标记为 API 相关警告
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
                file_tampering=file_tampering,
                snapshot=snapshot,
                cleanup_errors=cleanup_errors,
                total_test_failures=total_failures,
            )

        finally:
            # ---- 步骤 13: 清理 + 验证 ----
            if workspace_dir and workspace_dir.exists():
                try:
                    shutil.rmtree(workspace_dir)
                except (PermissionError, OSError):
                    # Docker 容器创建的文件可能由 root 所有，
                    # Python 删不掉时用 Docker 容器强制删除
                    subprocess.run(
                        ["docker", "run", "--rm", "--user", "0:0",
                         "-v", f"{workspace_dir}:/ws:rw",
                         "--entrypoint", "sh", DOCKER_IMAGE,
                         "-c", "chmod -R a+rwX /ws 2>/dev/null; "
                               "rm -rf /ws/* /ws/.[!.]* /ws/..?* 2>/dev/null; "
                               f"chown -R {os.getuid()}:{os.getgid()} /ws"],
                        capture_output=True, timeout=60,
                    )

            # 验证清理结果
            cleanup_errors = self._verify_cleanup(workspace_dir)
            if cleanup_errors:
                for err in cleanup_errors:
                    print(f"[CLEANUP WARNING] {err}")

            # git 重置回干净状态
            hard_reset_repo(self.repo_root, clean=True)

    # ==================================================================
    # 工作区准备
    # ==================================================================

    def _prepare_workspace(self, task):
        """为每个 task 创建隔离的工作目录。

        目录结构:
          /home/carl/.riot-workspaces/task_{id}/
            repo/            ← RIOT 源码副本（Claude 在此目录内工作）
            claude_home/     ← Claude 的 .claude/ 目录（含 settings.json）

        权限策略：
          - 所有目录 chmod 555 → 不能创建/删除/重命名文件（Linux 删文件看目录权限）
          - 所有文件 chmod a-w → 不能修改（保留可执行位，供构建脚本用）
          - 目标文件 chmod 644 → 唯一可写的文件
          - BUILD_DIR=/tmp/build 处理构建时 mkdir 需求

        .git 目录不会被复制到工作区，且复制后显式验证已移除，
        防止 Claude 用 git 还原被 mask 的代码。

        Args:
            task: task dict
        Returns:
            workspace_dir 的 Path
        """
        task_id = task["task_id"]
        ws_dir = WORKSPACE_BASE / f"task_{task_id}"
        repo_dir = ws_dir / "repo"
        claude_home = ws_dir / "claude_home"

        # 清理遗留的工作区
        # 先尝试普通 rmtree；失败则用 Docker root 容器清理内容（不能 rm -rf /ws
        # 因为 bind mount 的根目录不能被删除，只能删内容）。
        if ws_dir.exists():
            try:
                shutil.rmtree(ws_dir)
            except (PermissionError, OSError):
                subprocess.run(
                    ["docker", "run", "--rm", "--user", "0:0",
                     "-v", f"{ws_dir}:/ws:rw",
                     "--entrypoint", "sh", DOCKER_IMAGE,
                     "-c", "chmod -R a+rwX /ws 2>/dev/null; "
                           "rm -rf /ws/* /ws/.[!.]* /ws/..?* 2>/dev/null; "
                           f"chown -R {os.getuid()}:{os.getgid()} /ws"],
                    capture_output=True, timeout=60,
                )
            if ws_dir.exists():
                # Clean up residual empty dir
                try:
                    import shutil as _shutil
                    _shutil.rmtree(ws_dir)
                except Exception:
                    pass

        repo_dir.mkdir(parents=True, exist_ok=True)
        claude_home.mkdir(parents=True, exist_ok=True)

        # 用 shutil.copytree 复制 RIOT 源码（排除 .git 和 riot-claude 自身）
        try:
            shutil.copytree(
                self.repo_root, repo_dir,
                ignore=shutil.ignore_patterns('.git', 'riot-claude'),
                dirs_exist_ok=True,
            )
        except (OSError, shutil.Error) as e:
            raise RuntimeError(f"Failed to copy repo to workspace: {e}")

        # 显式验证 .git 未被复制到工作区（防止 Claude 用 git 还原 mask 代码）
        git_dir = repo_dir / ".git"
        if git_dir.exists():
            shutil.rmtree(git_dir)

        # ---- 删除 tests/ 目录 ----
        # Agent 不能看到测试断言/oracle，否则可能硬编码输出。
        if not task["source_path"].startswith("tests/"):
            tests_dir = repo_dir / "tests"
            if tests_dir.exists():
                shutil.rmtree(tests_dir)

        # ---- 删除文档/示例目录 ----
        # 减少 Claude 通过文档、示例代码找到参考实现的可能性。
        for extra_dir in ["docs", "examples", "samples"]:
            d = repo_dir / extra_dir
            if d.exists():
                shutil.rmtree(d)

        # ---- tree-sitter mask ----
        # 将目标函数体替换为 /* MASKED */
        ws_file = repo_dir / task["source_path"]
        apply_mask(ws_file, task["sut_function"], task["masked_code"])

        # ---- 复制 Claude settings + 注入 hooks ----
        # 从宿主机复制 API key、model 配置，同时注入 claude-on-completion hook
        # hook 在 Claude 完成任务时自动触发，比依赖 LLM 输出完成标记更可靠
        if CLAUDE_SETTINGS_SRC.exists():
            settings = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            settings.setdefault("hooks", {})["claude-on-completion"] = (
                "echo '=== RIOT_TASK_COMPLETED ==='"
            )
            (claude_home / "settings.json").write_text(
                json.dumps(settings, indent=2, ensure_ascii=False)
            )

        # ---- 基础权限 ----
        # 容器以宿主 UID 运行，需要确保文件至少可读
        ret = subprocess.run(
            ["chmod", "-R", "a+rX", str(repo_dir)],
            capture_output=True,
        )
        if ret.returncode != 0:
            err = (ret.stderr or ret.stdout or b"").decode(errors="replace")
            raise RuntimeError(f"chmod a+rX failed on repo_dir: {err}")
        ret = subprocess.run(
            ["chmod", "-R", "a+rwX", str(claude_home)],
            capture_output=True,
        )
        if ret.returncode != 0:
            err = (ret.stderr or ret.stdout or b"").decode(errors="replace")
            raise RuntimeError(f"chmod a+rwX failed on claude_home: {err}")

        # ---- 工作区锁定 ----
        # 目录设为 555，彻底阻止 Claude 创建/删除/重命名文件。
        # Linux 删除文件看目录写权限，不看文件权限——755 目录下
        # 即使文件是 a-w，Claude 也能 rm + 重建。555 从源头阻断。
        # BUILD_DIR=/tmp/build 处理构建时 mkdir 需求。
        ret = subprocess.run(
            ["find", str(repo_dir), "-type", "d",
             "-exec", "chmod", "555", "{}", "+"],
            capture_output=True,
        )
        if ret.returncode != 0:
            err = (ret.stderr or ret.stdout or b"").decode(errors="replace")
            raise RuntimeError(f"chmod 555 on directories failed: {err}")
        # 所有文件去掉写权限（保留可执行位，shell/python 脚本仍可运行）
        ret = subprocess.run(
            ["find", str(repo_dir), "-type", "f",
             "-exec", "chmod", "a-w", "{}", "+"],
            capture_output=True,
        )
        if ret.returncode != 0:
            err = (ret.stderr or ret.stdout or b"").decode(errors="replace")
            raise RuntimeError(f"chmod a-w on files failed: {err}")
        # 唯一可写的文件：目标源文件
        target_file = repo_dir / task["source_path"]
        target_file.chmod(0o644)

        return ws_dir

    # ==================================================================
    # 环境快照
    # ==================================================================

    def _capture_snapshot(self, task, repo_dir, dataset_paths=None):
        """记录完整环境快照，用于日后复现本次实验。

        快照内容：
          - git HEAD commit hash
          - Docker 镜像 digest + ID
          - Claude model 版本
          - Claude Code CLI 版本
          - Prompt SHA256 + 原文
          - 完整 task dict
          - 数据集文件 SHA256
          - Docker 环境变量
          - 依赖锁文件 hash
          - tree-sitter grammar 文件 hash

        这些数据随结果一起写入 results.jsonl，使得每个 task 的结果
        都可以追溯到其运行时的精确环境。

        Args:
            task: task dict
            repo_dir: 工作区 repo 路径
            dataset_paths: 可选，数据集 JSONL 文件路径列表
        Returns:
            dict: 快照数据
        """
        snap = {}

        # Git HEAD
        r = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            capture_output=True, text=True, cwd=self.repo_root,
        )
        snap["git_head"] = r.stdout.strip() if r.returncode == 0 else None

        # Docker 镜像 digest（索引标签摘要，含 registry 信息）
        r = subprocess.run(
            ["docker", "inspect", "--format={{index .RepoDigests 0}}",
             DOCKER_IMAGE],
            capture_output=True, text=True,
        )
        snap["docker_image_digest"] = r.stdout.strip() if r.returncode == 0 else None

        # Docker 镜像 content-addressed ID（不可变，即使 tag 被覆盖也不会变）
        r = subprocess.run(
            ["docker", "inspect", "--format={{.Id}}", DOCKER_IMAGE],
            capture_output=True, text=True,
        )
        snap["docker_image_id"] = r.stdout.strip() if r.returncode == 0 else None

        # Claude model
        try:
            cfg = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            snap["claude_model"] = cfg.get("model", "unknown")
        except Exception:
            snap["claude_model"] = "unknown"

        # Claude Code CLI 版本（不同版本行为可能差异显著）
        r = subprocess.run(
            ["docker", "run", "--rm", "--entrypoint", "claude",
             DOCKER_IMAGE, "--version"],
            capture_output=True, text=True,
        )
        snap["claude_code_version"] = r.stdout.strip() if r.returncode == 0 else None

        # Prompt hash
        prompt = self._build_prompt(task)
        snap["prompt_sha256"] = hashlib.sha256(prompt.encode()).hexdigest()
        snap["prompt_template_sha256"] = hashlib.sha256(
            TASK_PROMPT.encode()
        ).hexdigest()

        # Docker 环境变量（脱敏后存储，防止 API key 泄露到 results.jsonl）
        env_list = self._claude_env_args()
        snap["claude_env_vars"] = re.sub(
            r'=(\S+)',
            lambda m: '=' + (m.group(1)[:4] + '...' + m.group(1)[-4:]
                             if len(m.group(1)) > 10 else '***'),
            " ".join(env_list),
        )

        # Prompt 文本（除 hash 外，保存原文以便日后追溯）
        snap["prompt_text"] = prompt

        # 完整 task dict（含 source_path, sut_function, unit_test, run_command 等）
        # 确保每个结果独立可追溯，不依赖外部 task 文件
        snap["task"] = task

        # 依赖锁文件 hash
        for lockfile in ["Makefile", "Makefile.base", "Makefile.include"]:
            p = repo_dir / lockfile
            if p.exists():
                snap[f"{lockfile}_sha256"] = hashlib.sha256(
                    p.read_bytes()
                ).hexdigest()

        # tree-sitter grammar 文件 hash（parser 版本影响 AST 解析结果）
        ts_lib = BENCH_ROOT / "build" / "my-languages.so"
        if ts_lib.exists():
            snap["tree_sitter_grammar_sha256"] = hashlib.sha256(
                ts_lib.read_bytes()
            ).hexdigest()
        else:
            snap["tree_sitter_grammar_sha256"] = None

        # 数据集文件 hash（task 来源，日后复现需要知道用的是什么数据集版本）
        if dataset_paths:
            dataset_hashes = {}
            for dp in sorted(dataset_paths):
                dp = Path(dp)
                if dp.exists():
                    h = hashlib.sha256(dp.read_bytes()).hexdigest()
                    dataset_hashes[str(dp)] = h
            snap["dataset_sha256"] = dataset_hashes

        return snap

    # ==================================================================
    # Prompt 构建
    # ==================================================================

    def _build_prompt(self, task):
        """生成发给 Claude 的任务描述。

        用 task 中的函数名和文件路径填充 prompt 模板。
        """
        return TASK_PROMPT.format(
            sut_function=task["sut_function"],
            source_path=task["source_path"],
        )

    # ==================================================================
    # Docker Claude 执行
    # ==================================================================

    def _snapshot_files(self, repo_dir, previous=None):
        """扫描工作区文件，返回 {相对路径: (sha256, mtime, size)} 字典。

        用 SHA256 内容哈希做指纹（size 和 mtime 可以被恢复，SHA256 无法伪造）。
        当传入 previous 快照时，对 mtime 和 size 均未变的文件复用旧 SHA256，
        避免重复哈希大量未变更文件。

        Args:
            repo_dir: 工作区路径
            previous: 可选的上一轮快照，用于 mtime+size 预筛选
        Returns:
            dict: {relpath: (sha256_hex, mtime_float, size_int)}
        """
        result = subprocess.run(
            ["find", str(repo_dir), "-type", "f",
             "-printf", "%P\\0%s\\0%T@\\0"],
            capture_output=True, text=True,
        )
        prev = previous or {}
        snap = {}
        # Output format per file: relpath\0size\0mtime\0
        # Split on \0 → tokens in groups of 3
        tokens = result.stdout.split("\0")
        for i in range(0, len(tokens) - 2, 3):
            relpath = tokens[i]
            size_str = tokens[i + 1]
            mtime_str = tokens[i + 2]
            if not relpath or not size_str or not mtime_str:
                continue
            mtime = float(mtime_str)
            size = int(size_str)

            # mtime+size 预筛选：均未变则复用旧 hash
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
        """比较两个文件快照，找出目标文件之外的变更。

        使用 mtime+size 做预筛选：均未变的文件直接跳过，避免不必要的 SHA256 比较。
        仅当 mtime 或 size 变化时才对比 SHA256 确认实际内容变更。

        Args:
            before: 运行前的文件快照 {relpath: (sha256, mtime, size)}
            after:  运行后的文件快照 {relpath: (sha256, mtime, size)}
            target_path: 允许被修改的目标文件路径
        Returns:
            list[str]: 被创建/修改/删除的文件描述列表
                       空列表 = 无违规
        """
        modified = []
        for path, (after_hash, after_mtime, after_size) in after.items():
            if path == target_path:
                continue
            if path not in before:
                modified.append(f"{path} (new)")
            else:
                before_hash, before_mtime, before_size = before[path]
                # mtime 和 size 均未变 → 内容不可能变，跳过 hash 比较
                if before_mtime == after_mtime and before_size == after_size:
                    continue
                if before_hash != after_hash:
                    modified.append(f"{path} (modified)")
        for path in before:
            if path != target_path and path not in after:
                modified.append(f"{path} (deleted)")
        return modified

    # 只允许传递给 Claude 容器的环境变量白名单
    # 避免将 settings.json env 块中的非预期变量（HOME, PATH 等）注入容器，
    # 影响可复现性和隔离性。
    _ALLOWED_CLAUDE_ENV = {
        "ANTHROPIC_API_KEY", "ANTHROPIC_AUTH_TOKEN", "ANTHROPIC_BASE_URL",
        "ANTHROPIC_MODEL",
        "ANTHROPIC_DEFAULT_OPUS_MODEL", "ANTHROPIC_DEFAULT_SONNET_MODEL", "ANTHROPIC_DEFAULT_HAIKU_MODEL",
    }

    @staticmethod
    def _claude_env_args():
        """从 settings.json 读取 Claude API 环境变量，生成 Docker -e 参数列表。

        Claude Code 2.1.146+ 在非 TTY 模式下会忽略 settings.json 中的 "env" 块，
        因此需要通过 Docker 的 -e 参数显式传入环境变量。
        仅传递白名单中的变量（ANTHROPIC_API_KEY, ANTHROPIC_BASE_URL），
        防止非预期环境变量污染容器。

        返回示例: ["-e", "ANTHROPIC_API_KEY=xxx", "-e", "ANTHROPIC_BASE_URL=yyy"]
        如果读取失败则返回空列表。
        """
        try:
            cfg = json.loads(CLAUDE_SETTINGS_SRC.read_text())
            env = cfg.get("env", {})
            allowed = ClaudeAgent._ALLOWED_CLAUDE_ENV
            args = []
            for k, v in env.items():
                if k in allowed:
                    args.extend(["-e", f"{k}={v}"])
            return args
        except Exception:
            return []

    def _run_claude(self, task, workspace_dir):
        """在 Docker 容器中执行 Claude Code CLI。

        执行方式：
          docker run ... --entrypoint bash riot-sandbox -c 'claude -p "$(cat)"'

        通过 stdin pipe 传入 prompt（而非命令行参数），
        避免 shell 转义和参数长度限制问题。

        限制：
          - --cap-drop ALL, --security-opt no-new-privileges
          - --pids-limit 256, --memory 4g, --cpus 2
          - --user $(id -u):$(id -g)（非 root）
          - tmpfs 挂载（限制临时文件大小）

        输出处理：
          - 实时读取并打印（strip ANSI 转义）
          - 检测 === RIOT_TASK_COMPLETED === 标记
          - claude-on-completion hook 检测完成

        Args:
            task: task dict
            workspace_dir: 工作区路径
        Returns:
            (claude_completed, full_output):
                claude_completed: bool, 是否检测到完成标记
                full_output: str, Claude 的完整 stdout 输出
        """
        prompt = self._build_prompt(task)
        repo_dir = workspace_dir / "repo"
        claude_home = workspace_dir / "claude_home"

        # Docker 命令构造（list 形式，避免 shell 注入风险）
        docker_args = [
            "docker", "run", "--rm", "-i",
            "--init",
            "--cap-drop", "ALL",
            "--security-opt", "no-new-privileges",
            "--pids-limit", DOCKER_PIDS_LIMIT,
            "--memory", DOCKER_MEMORY,
            "--cpus", DOCKER_CPUS,
        ]
        if CLAUDE_NETWORK_MODE:
            docker_args.extend(["--network", CLAUDE_NETWORK_MODE])
        docker_args.extend(_docker_user_args())
        docker_args.extend(self._claude_env_args())
        docker_args.extend([
            "-e", "BUILD_DIR=/tmp/build",
            "-e", "BINDIRBASE=/tmp/build/bin",
            "-e", "TERM=dumb",
            "-e", "NO_COLOR=1",
            "-e", "CI=1",
            "-e", "PAGER=cat",
            "-e", "GIT_PAGER=cat",
            "-v", f"{repo_dir}:/workspace:rw",
            "-v", f"{claude_home}:/home/agent/.claude:rw",
            "--tmpfs", "/tmp:size=1g,exec",
            "--tmpfs", "/home/agent/.cache:size=500m",
            "--tmpfs", "/home/agent/.config:size=100m",
            "-w", "/workspace",
            "--entrypoint", "bash",
            DOCKER_IMAGE,
            "-c", 'claude --dangerously-skip-permissions -p "$(cat)" 2>&1',
        ])

        print(f"  ┌─ {'='*60}")
        print(f"  │ Docker agent: {task['sut_function']} (task {task['task_id']})")
        print(f"  │ workspace: {repo_dir}")
        print(f"  └─ {'='*60}")

        # 启动 Docker 进程，通过 stdin pipe 写入 prompt
        proc = subprocess.Popen(
            docker_args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )

        # 将 prompt 写入 stdin，然后关闭
        proc.stdin.write(prompt)
        proc.stdin.close()

        # 使用 selectors 进行非阻塞读取
        # 裸 readline() 可能永久挂起（子进程卡死、Docker 死锁、pipe 未关闭等），
        # selectors + watchdog 解决的是"框架挂死"问题，不是限制 Agent 工作时间。
        # Agent 想跑多久就跑多久——只有进程长时间无输出（可能死锁）时才由 watchdog 终止。
        sel = selectors.DefaultSelector()
        sel.register(proc.stdout, selectors.EVENT_READ)

        output_chunks = []
        last_activity = time.time()
        claude_completed = False

        while True:
            # 检查进程是否已退出
            if proc.poll() is not None:
                break

            # Activity-based watchdog: 每次收到输出都刷新计时器
            # 只有长时间完全无输出时才判定为死锁
            idle = time.time() - last_activity
            if idle > WATCHDOG_TIMEOUT:
                proc.kill()
                output_chunks.append(
                    "\n[WATCHDOG] Task idle for "
                    f"{WATCHDOG_TIMEOUT}s with no output — likely deadlocked. Killed.\n"
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

                if re.search(r"^=== RIOT_TASK_COMPLETED ===$", clean.strip()):
                    claude_completed = True

        sel.unregister(proc.stdout)
        sel.close()

        # 排空残留输出（进程已退出但 pipe 可能还有缓冲数据）
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
            if re.search(r"^=== RIOT_TASK_COMPLETED ===$", clean.strip()):
                claude_completed = True

        proc.wait(timeout=10)
        full_output = "".join(output_chunks)

        # 完成判定：优先以 hook 标记为准，fallback 到 returncode。
        # hook 可能因 CLI 升级/schema 变更而失效，returncode 作为辅助。
        # claude_completed 本身只是辅助指标，不 gate PASS/FAIL。
        if not claude_completed and proc.returncode == 0:
            claude_completed = True

        if claude_completed:
            print(f"\n  │ Claude completed (hook or clean exit)")
        else:
            print(f"\n  │ Claude did not report completion (rc={proc.returncode})")

        return claude_completed, full_output

    # ==================================================================
    # 测试验证
    # ==================================================================

    def _verify_tests(self, task, workspace_dir):
        """在独立 Docker 容器中编译并运行单元测试。

        验证容器与 Claude 容器不同：
          - --network none（完全无网络）
          - 从原始仓库只读挂载 tests/（Claude 工作区中没有 tests/）

        判断逻辑：
          PASS = 编译成功 AND 无崩溃 AND 目标测试不在失败列表中
          FAIL = 编译失败 OR 段错误/panic OR 目标测试失败

        Args:
            task: task dict
            workspace_dir: 工作区路径
        Returns:
            (passed, test_output, total_failures, error_category):
                passed: bool
                test_output: str（stdout + stderr）
                total_failures: int（所有失败测试数，含非目标测试）
                error_category: str or None（失败时的错误分类）
        """
        repo_dir = workspace_dir / "repo"
        cmd = task["run_command"]

        # BUILD_DIR=/tmp/build 将构建产物重定向到 tmpfs，
        # 但 run_command 中的 ./bin/ 路径需要同步指向 /tmp/build/bin/。
        # 同时 cd /tmp 解决 mtd_native 在 CWD 创建 MEMORY.bin 的问题
        # （workspace 目录 555 不可写，必须在 tmpfs 中运行）。
        cmd = cmd.replace("&& ./bin/", "&& cd /tmp && /tmp/build/bin/")

        # 对于非 test task，验证时挂载 tests/ 到 /riot_tests 然后复制到 workspace，
        # 而不直接挂载或 symlink。symlink 会破坏 RIOT Makefile 的相对路径解析；
        # 直接挂载会产生 root-owned 目录干扰后续清理。
        tests_mount = []
        tests_setup = ""
        if not task["source_path"].startswith("tests/"):
            tests_mount = ["-v", f"{self.repo_root / 'tests'}:/riot_tests:ro"]
            tests_setup = "cp -r /riot_tests /workspace/tests && "

        docker_args = [
            "docker", "run", "--rm", "-i",
            "--network", "none",
            "--cap-drop", "ALL",
            "--security-opt", "no-new-privileges",
            "--pids-limit", DOCKER_PIDS_LIMIT,
            "--memory", DOCKER_MEMORY,
            "--cpus", DOCKER_CPUS,
            *_docker_user_args(),
            "-e", "BUILD_DIR=/tmp/build",
            "-e", "BINDIRBASE=/tmp/build/bin",
            "-v", f"{repo_dir}:/workspace:rw",
            *tests_mount,
            "--tmpfs", "/tmp:size=1g,exec",
            "-w", "/workspace",
            "--entrypoint", "bash",
            DOCKER_IMAGE,
            "-c", (
                # No set -e: make returns non-zero on test failure,
                # and we need to capture output to classify the failure.
                f"export LSAN_OPTIONS=detect_leaks=0; "
                f"find /workspace -type d -exec chmod 755 {{}} + 2>/dev/null; "
                f"find /workspace -type f -name '*.sh' -exec chmod 755 {{}} + 2>/dev/null; "
                f"find /workspace -type f -name '*.py' -exec chmod 755 {{}} + 2>/dev/null; "
                # Offline package support: the verify container has --network none,
                # so git clone/fetch from pkg.mk will fail.  Both the simple
                # cp (populates the BUILD_DIR cache) and PKG_SOURCE_LOCAL_<NAME>
                # (bypasses git entirely via pkg/local.mk) are needed because
                # some package flows rm -rf PKG_SOURCE_DIR before cloning.
                f"mkdir -p /tmp/build/pkg && "
                f"cp -r /workspace/build/pkg/* /tmp/build/pkg/ 2>/dev/null; "
                f"for pkg_dir in /workspace/build/pkg/*/; do "
                f"[ -d \"$pkg_dir\" ] || continue; "
                f"pkg_name=$(basename \"$pkg_dir\"); "
                f"pkg_var_suffix=$(echo \"$pkg_name\" | tr 'a-z-' 'A-Z_'); "
                f"export \"PKG_SOURCE_LOCAL_${{pkg_var_suffix}}=$pkg_dir\"; "
                f"done; "
                f"{tests_setup}"
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

            # ---- 检查 1: 编译失败 ----
            build_fail = bool(
                re.search(
                    r"(?:undefined reference to|"
                    r"collect2:\s*error|"
                    r"compilation terminated\.|"
                    r"make(?:\[\d+\])?:\s*\*\*\*)",
                    full,
                )
            )

            # ---- 检查 2: 崩溃/致命错误 ----
            fatal_fail = bool(
                re.search(
                    r"(?:Segmentation\s+fault|Aborted|\bpanic\b)",
                    full,
                )
            )

            # ---- 检查 3: 目标测试是否在失败列表中 ----
            # embUnit 失败输出格式: "suite.test_name (file.c:NN) exp X was Y"
            # 例如: "mtd_tests.test_mtd_erase (tests/unittests/tests-mtd/tests-mtd.c 64) exp 0 was -5"
            # 如果目标测试未被执行，不会出现在失败列表中，
            # 此时 specific_test_failed=False 即为正确结果。
            # \b 边界防止 test_foo 误匹配 test_foo_bar
            target_test = task.get("unit_test", "")
            specific_test_failed = False
            if target_test:
                specific_test_failed = bool(re.search(
                    rf'\.{re.escape(target_test)}\b\s*\(',
                    stdout,
                ))

            # ---- 全量失败计数 ----
            # 统计所有 failure 行（含非目标测试），用于分析时区分
            # "目标测试通过但其他测试挂了" vs "目标测试也挂了"
            total_failures = len(re.findall(
                r'^\w+\.\w+\s*\(.*\)\s+exp\s+\d+',
                stdout,
                re.MULTILINE,
            ))

            # ---- 综合判断 + 错误分类 ----
            passed = (
                not build_fail
                and not fatal_fail
                and not specific_test_failed
            )

            error_cat = None
            if build_fail:
                passed = False
                error_cat = ERROR_COMPILE
            elif fatal_fail:
                passed = False
                error_cat = ERROR_CRASH
            elif specific_test_failed:
                passed = False
                error_cat = ERROR_TEST_FAIL

            # 额外守卫：检测测试是否真的执行了。
            if passed and not task["source_path"].startswith("tests/"):
                tests_ran = bool(re.search(
                    r'(?:^OK\s*\(\d+\s+tests?\)|'
                    r'^FAILED\s*\(\d+\s+tests?\)|'
                    r'^\w+\.\w+\s*\()',
                    stdout,
                    re.MULTILINE,
                ))
                shell_fail = bool(re.search(
                    r'(?:No such file or directory|'
                    r'command not found|'
                    r'cannot execute|cannot open)',
                    stderr,
                ))
                if not tests_ran or shell_fail:
                    passed = False
                    error_cat = ERROR_TEST_NOT_RUN
                    test_out += (
                        "\n[HARNESS] Test binary may not have executed — "
                        "no embUnit output or shell error detected.\n"
                    )

            return passed, test_out, total_failures, error_cat

        except subprocess.TimeoutExpired:
            return False, "[TIMEOUT] Test command exceeded timeout.", -1, ERROR_TIMEOUT

    # ==================================================================
    # 清理验证
    # ==================================================================

    def _verify_cleanup(self, workspace_dir):
        """验证清理是否彻底，返回发现的问题列表。

        检查项：
          1. 工作区目录是否已删除
          2. 是否有残留的 Docker 容器
          3. git 仓库状态是否干净

        任何失败都不静默跳过，全部记入结果。

        Args:
            workspace_dir: 被删除的工作区路径（可能为 None）
        Returns:
            list[str]: 清理问题描述列表，空列表 = 清理干净
        """
        errors = []

        # 检查 1: 工作区目录
        if workspace_dir and workspace_dir.exists():
            errors.append(f"workspace directory still exists: {workspace_dir}")

        # 检查 2: 残留 Docker 容器
        r = subprocess.run(
            ["docker", "ps", "-a", "--format", "{{.Names}}"],
            capture_output=True, text=True,
        )
        if r.returncode == 0:
            for name in r.stdout.strip().split("\n"):
                if not name:
                    continue
                if name.startswith("task_") or "riot" in name.lower():
                    errors.append(f"lingering Docker container: {name}")

        # 检查 3: git 仓库干净（排除 riot-claude/，它是 benchmark 工具本身）
        r = subprocess.run(
            ["git", "status", "--porcelain"],
            capture_output=True, text=True, cwd=self.repo_root,
        )
        if r.returncode == 0 and r.stdout.strip():
            dirty = [
                x for x in r.stdout.strip().split("\n")
                if x.strip() and "riot-claude" not in x
            ][:5]
            if dirty:
                errors.append(f"git repo not clean after reset: {dirty}")

        return errors

    # ==================================================================
    # 日志 / 结果
    # ==================================================================

    def _record_trajectory(
        self, task, claude_out, diff, illegal,
    ):
        """将 Claude session 输出、diff、违规信息写入轨迹日志。

        轨迹文件 = trajectory/task_{task_id}.log

        Claude 输出超长时（> MAX_OUTPUT_LENGTH），
        截断为"开头一半 + ... + 结尾一半"的形式。
        """
        traj_file = TRAJECTORY_DIR / f"task_{task['task_id']}.log"

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
        file_tampering=None, snapshot=None,
        cleanup_errors=None, total_test_failures=None,
    ):
        """构造标准化的 task 结果字典。

        Args:
            task: task dict
            start: time.time() 起始时间
            passed: 是否通过
            claude_completed: Claude 是否报告完成（辅助指标）
            illegal: 违规列表（空 = 无违规）
            error: 错误描述字符串（人读）
            error_category: 错误分类（机读，如 "compile_error"）
            diff: unified diff 字符串
            file_tampering: 文件篡改列表
            snapshot: 环境快照 dict（用于日后复现）
            cleanup_errors: 清理验证发现的问题列表
            total_test_failures: 全量失败测试数（含非目标测试）
        Returns:
            dict
        """
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

        if cleanup_errors:
            result["cleanup_errors"] = cleanup_errors

        if total_test_failures is not None:
            result["total_test_failures"] = total_test_failures

        return result
