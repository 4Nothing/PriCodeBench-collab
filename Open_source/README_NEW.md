# PriCodeBench: 嵌入式系统代码补全能力基准测试

## 这个项目是什么？

这是一个 **自动化基准测试框架**，用于评估 AI Agent 补全嵌入式 C 函数的能力。

核心思路：从 RIOT 实时操作系统中选取 538 个真实 C 函数，把函数体挖空（mask），让 AI Agent 在隔离的 Docker 沙箱中阅读源码上下文并补全实现，然后通过编译 + 单元测试验证正确性。

---

## 仓库结构

```
PriCodeBench-collab/
├── RIOT/                        ← RIOT-OS 实时操作系统源码（含 538 个 benchmark 目标函数）
│   ├── core/ sys/ drivers/ …   ← 内核、系统库、驱动等
│   │
│   ├── riot-claude/             ← ★ 基准测试框架核心
│   │   ├── agent.py             ← 核心 Pipeline（13 步任务执行流程）
│   │   ├── runner.py            ← CLI 入口（单任务 / 批量 / 断点续跑）
│   │   ├── mask.py              ← tree-sitter AST 操作（挖空函数体 + 完整性校验）
│   │   ├── config.py            ← 全局配置
│   │   ├── prompt.py            ← 发给 AI Agent 的任务描述模板
│   │   ├── git_utils.py         ← Git 仓库重置（task 间隔离）
│   │   ├── logger.py            ← 轨迹日志
│   │   ├── analyze_results.py   ← 结果统计与分析
│   │   ├── Dockerfile           ← 沙箱镜像（Ubuntu + 编译工具链 + Claude CLI）
│   │   ├── WORKFLOW.md          ← 13 步流程详解（每一步的 WHY）
│   │   ├── 结果.md              ← 最新一轮测试结果报告
│   │   ├── results/             ← 结构化运行结果（JSONL）
│   │   ├── trajectory/          ← 每个 task 的完整执行日志
│   │   ├── *.verified.jsonl     ← 538 个已验证任务（305 .c + 234 .h）
│   │   └── .claude/             ← Claude Code 配置
│   │
│   └── tests/                   ← RIOT 单元测试（Agent 看不到）
│
└── riot-bench/                  ← 数据集构建 & 验证工具
    ├── build/                   ← tree-sitter C grammar 编译产物（my-languages.so）
    ├── build_dataset.py         ← 从 RIOT 源码提取函数、生成 task
    ├── verify_tasks.py          ← tree-sitter AST 验证 task ↔ test 映射
    ├── extract_unit_tests.py    ← 提取所有单元测试用例
    └── mask_engine.py           ← 函数体 mask 引擎
```

---

## 核心设计

### 设计原则

> **AI Agent 是纯黑盒 — 只负责读代码、写代码，不跑测试、不编译。**

所有验证逻辑由 Python 框架在独立的断网 Docker 容器中完成。

### 13 步 Pipeline

```
准备 (1–4) → 执行 (5) → 校验 (6–9) → 测试 (10) → 收尾 (11–13)

  1. git reset --hard + clean   6. 文件完整性检查（SHA256 快照对比）
  2. 工作区副本 + mask + 权限锁定  7. 读取最终源码
  3. 环境快照（git/docker/model） 8. unified diff
  4. 文件快照（基线指纹）         9. AST 归一化校验 + 关键字检查
  5. Docker 内运行 Claude
                               10. 独立容器编译 + 单元测试（--network none）

                               11. 轨迹日志 → 12. 结构化结果 → 13. 清理 + 验证
```

### 三层防作弊

| 层 | 机制 | 防止 |
|---|---|---|
| 工作区锁定 | 目录 555 + 文件 a-w + 目标 644 | 修改非目标文件 |
| 文件完整性 | 前后 SHA256 快照对比 | 创建/删除/修改其他文件 |
| AST 归一化 | 函数体占位 → 参数泛型化 → 签名归一化 → 逐字对比 | 改签名、全局变量、预处理作弊 |

### 容器隔离

- Claude 容器：`--cap-drop ALL`, 非 root, 工作区无 `.git/`, 无 `tests/`
- 验证容器：同上 + `--network none`（完全断网）

---

## 数据集

538 个任务，分布在 `RIOT/riot-claude/` 下：

| 文件 | 数量 | 说明 |
|---|---|---|
| `riot_tasks_with_commands.c.verified.jsonl` | 305 | `.c` 源文件函数 |
| `riot_tasks_with_commands.h.verified.jsonl` | 234 | `.h` 头文件中的静态 inline 函数 |

每条任务：

```json
{
  "task_id": "1",
  "source_path": "sys/analog_util/adc_util.c",
  "sut_function": "adc_util_map",
  "unit_test": "test_adc_util_map",
  "run_command": "make -C tests/unittests BOARD=native64 tests-analog_util ..."
}
```

所有任务经 tree-sitter AST 验证，确保 `unit_test` 确实存在且包含 `TEST_ASSERT*` 断言。

---

## 使用方法

在 WSL2 (Ubuntu) 内执行：

```bash
cd RIOT/riot-claude

# 构建 Docker 镜像
docker build -t riot-sandbox:latest .

# 检查依赖
python3 runner.py --check

# 单任务 / 批量 / 断点续跑
python3 runner.py --task-id 1
python3 runner.py --batch 10
python3 runner.py --batch 20 --resume

# 分析结果
python3 analyze_results.py
```

---

## 当前结果

使用 DeepSeek V4 Pro：

| 指标 | 数值 |
|---|---|
| 总任务 | 538 |
| 通过率 | **90.7%** (488/538) |
| 头文件 (.h) | 95.3% |
| 源文件 (.c) | 87.2% |
| 中位数耗时 | 48s |

主要失分：API 幻觉、空指针防御不足、网络协议栈耦合深。详见 `RIOT/riot-claude/结果.md`。

---

## 运行环境

| 组件 | 说明 |
|---|---|
| WSL2 (Ubuntu) | 宿主机 |
| Docker Desktop | WSL2 backend |
| Python 3.10+ | + `tree-sitter` |
| Claude Code CLI | Docker 镜像内（npm 全局，v2.1.146） |
| Claude API Key | Windows 侧 `%USERPROFILE%\.claude\settings.json` |

---

## 相关文件索引

- **理解流程设计** → `RIOT/riot-claude/WORKFLOW.md`
- **调整配置** → `RIOT/riot-claude/config.py`
- **修改 prompt** → `RIOT/riot-claude/prompt.py`
- **理解 mask 逻辑** → `RIOT/riot-claude/mask.py`
- **查看完整结果** → `RIOT/riot-claude/结果.md`
- **遍历数据集** → `RIOT/riot-claude/*.verified.jsonl`
- **重建 Docker 镜像** → `RIOT/riot-claude/Dockerfile`
