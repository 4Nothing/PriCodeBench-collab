"""
Git 操作工具 —— 在每个 task 前后重置 RIOT 仓库状态。

hard_reset_repo() 确保 task 之间完全隔离：
  - git reset --hard HEAD : 丢弃所有本地修改（Claude 对 workspace 的修改不影响原始仓库）
  - git clean -fd         : 删除所有未跟踪文件（编译产物等）

排除目录（不会被 clean 删除）:
  - riot-claude/results/     : 累积的结果数据
  - riot-claude/trajectory/  : 累积的轨迹日志
  - riot-claude/.claude/     : Claude Code 的工作文件
  - .claude_home/            : Claude 的 HOME 目录
"""
import subprocess


def hard_reset_repo(repo_root, clean=False):
    """将 git 仓库重置到干净状态。

    Args:
        repo_root: RIOT 仓库路径
        clean: 是否同时执行 git clean -fd（删除未跟踪文件）
    """
    r = subprocess.run(
        ["git", "reset", "--hard", "HEAD"],
        cwd=repo_root,
        capture_output=True,
    )
    if r.returncode != 0:
        err = (r.stderr or r.stdout or b"").decode(errors="replace").strip()
        raise RuntimeError(f"git reset --hard failed: {err}")

    if clean:
        r = subprocess.run(
            ["git", "clean", "-fd",
             "-e", "riot-claude",
             "-e", ".claude_home",
             "-e", "build/pkg"],
            cwd=repo_root,
            capture_output=True,
        )
        if r.returncode != 0:
            err = (r.stderr or r.stdout or b"").decode(errors="replace").strip()
            raise RuntimeError(f"git clean -fd failed: {err}")
