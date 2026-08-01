"""
QSem-Claude 全局配置 (Docker edition)
"""
import os
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
QSemOS_ROOT = SCRIPT_DIR.parent
PRIVATE_CODE = QSemOS_ROOT.parent
ORACLES_DIR = PRIVATE_CODE / "oracles"

TEST_TIMEOUT = 120
WATCHDOG_TIMEOUT = 8 * 3600
MAX_OUTPUT_LENGTH = 2_000_000

TRAJECTORY_DIR = SCRIPT_DIR / "trajectory"
RESULTS_DIR = SCRIPT_DIR / "results"

CLAUDE_SETTINGS_SRC = Path(os.environ.get(
    "QSEM_CLAUDE_SETTINGS",
    str(Path.home() / ".claude" / "settings.json"),
))

DATASET_PATH = Path(os.environ.get(
    "QSEM_DATASET",
    str(PRIVATE_CODE / "PrivateAPIEval_agent.jsonl"),
))

CHECK_TESTS_DIR = QSemOS_ROOT / "check_tests"
TASKS_DIR = CHECK_TESTS_DIR / "tasks"

# =========================================================================
# Docker 配置
# =========================================================================

# 工作区根目录（隔离副本存放位置）
WORKSPACE_BASE = Path(
    os.environ.get("QSEM_WORKSPACE_BASE", "/tmp/qsem-workspaces")
)

# Docker 镜像（需先执行: docker build -t qsem-sandbox .）
DOCKER_IMAGE = os.environ.get("QSEM_DOCKER_IMAGE", "riot-sandbox:latest")

# Claude 容器的网络模式（默认 none = 彻底断网）
CLAUDE_NETWORK_MODE = os.environ.get("QSEM_CLAUDE_NETWORK", "host")

# 容器资源限制
DOCKER_PIDS_LIMIT = "256"
DOCKER_MEMORY = os.environ.get("QSEM_DOCKER_MEMORY", "4g")
DOCKER_CPUS = os.environ.get("QSEM_DOCKER_CPUS", "2")

# 只允许传递给 Claude 容器的环境变量白名单
ALLOWED_CLAUDE_ENV = {
    "ANTHROPIC_API_KEY", "ANTHROPIC_AUTH_TOKEN", "ANTHROPIC_BASE_URL",
    "ANTHROPIC_MODEL",
    "ANTHROPIC_DEFAULT_OPUS_MODEL", "ANTHROPIC_DEFAULT_SONNET_MODEL",
    "ANTHROPIC_DEFAULT_HAIKU_MODEL",
}

# ---- RAG ----
RAG_SOURCE_DIR = QSemOS_ROOT / "src"
RAG_DB_PATH = Path(os.environ.get("RAG_DB_PATH", SCRIPT_DIR / "rag_index" / "index.db"))
RAG_INDEX_DIR = Path(os.environ.get("RAG_INDEX_DIR", SCRIPT_DIR / "rag_index"))
