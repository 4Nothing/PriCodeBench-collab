"""
QSem-Claude 全局配置
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
