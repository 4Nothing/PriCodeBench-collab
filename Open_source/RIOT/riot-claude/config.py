"""
RIOT-Claude 全局配置

所有路径、超时、Docker 资源限制等集中在这里管理。
含用户名/路径的值优先从环境变量读取，方便不同研究者使用。
"""
import os
from pathlib import Path

# 本文件所在目录 = riot-claude/
SCRIPT_DIR = Path(__file__).resolve().parent
# RIOT 仓库根目录 = riot-claude/ 的父目录
RIOT_ROOT = SCRIPT_DIR.parent
# riot-bench 仓库（存放 tree-sitter 编译产物 my-languages.so）
BENCH_ROOT = RIOT_ROOT.parent / "riot-bench"

TEST_TIMEOUT = 300              # 验证容器的编译+测试超时（秒）
WATCHDOG_TIMEOUT = 8 * 3600     # 安全保险丝（秒）：防止单个异常 task 把整个 benchmark 卡死
                                # 这不是 Agent 的任务预算——Agent 想跑多久跑多久
                                # 只有 Claude CLI 死锁、Docker 卡死、pipe 挂起等异常才触发

# 轨迹日志中 Claude 输出截断阈值（字节）
# 防止单个 task 的日志撑爆磁盘
MAX_OUTPUT_LENGTH = 2_000_000

# ---- 目录 ----
TRAJECTORY_DIR = SCRIPT_DIR / "trajectory"    # 每次运行的完整日志
RESULTS_DIR = SCRIPT_DIR / "results"          # 结构化结果 JSONL

# ---- Docker 沙箱镜像 ----
DOCKER_IMAGE = "riot-sandbox:latest"

# Workspace 目录（在 WSL 内部）
# 每个 task 独立目录: task_{id}/repo, task_{id}/claude_home
WORKSPACE_BASE = Path(os.environ.get(
    "RIOT_WORKSPACE_BASE",
    str(Path.home() / ".riot-workspaces"),
))

# Claude settings.json 路径
# WSL 下 Claude 通常安装在 Windows 侧，settings.json 在 /mnt/c/Users/<user>/.claude/
# 如果检测到 WSL 环境且 Windows 侧存在 settings.json，优先使用；否则回退到 ~/.claude/
def _default_claude_settings():
    home = Path.home()
    if os.name == "posix":
        # WSL: WSL 用户名可能与 Windows 用户名不同，
        # 扫描 /mnt/c/Users/* 查找 settings.json
        users_dir = Path("/mnt/c/Users")
        if users_dir.exists():
            for user_dir in sorted(users_dir.iterdir()):
                candidate = user_dir / ".claude" / "settings.json"
                if candidate.exists():
                    return str(candidate)
        # 回退到 WSL home
        local = home / ".claude" / "settings.json"
        if local.exists():
            return str(local)
    return str(home / ".claude" / "settings.json")

CLAUDE_SETTINGS_SRC = Path(os.environ.get(
    "RIOT_CLAUDE_SETTINGS",
    _default_claude_settings(),
))

# ---- Claude 容器网络模式 ----
# "host" = 继承宿主机网络（默认，Claude CLI 需要调 API）
# "none" = 完全无网络（纯代码补全模式，只靠模型知识不联网搜索）
CLAUDE_NETWORK_MODE = os.environ.get("RIOT_CLAUDE_NETWORK", "host")

# ---- Docker 容器资源限制 ----
DOCKER_MEMORY = "4g"          # 最大内存
DOCKER_CPUS = "2"             # 可用 CPU 核数
DOCKER_PIDS_LIMIT = "0"       # 最大进程数（0 = 不限，unittests 并行编译需要大量进程）

# ---- 数据集默认文件 ----
# 可通过 --data CLI 参数覆盖
DEFAULT_TASK_FILES = [
    str(SCRIPT_DIR / "riot_tasks_with_commands.c.verified.jsonl"),
    str(SCRIPT_DIR / "riot_tasks_with_commands.h.verified.jsonl"),
]

# ---- Windows WSL 相关 ----
WSL_DISTRO = "Ubuntu"         # WSL 发行版名称
CONDA_INIT = ""               # Shell 初始化（Docker 模式下不需要）
