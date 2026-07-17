# RIOT-Claude: 自动化嵌入式系统基准测试框架

## 概述

RIOT-Claude 是一个 SWE-bench 风格的自动化基准测试框架，用于在 [RIOT](https://github.com/RIOT-OS/RIOT) 实时操作系统上评估 Claude Code Agent 补全 C 函数的能力。

框架将目标函数体挖空（mask），让 Claude Code CLI 在 Docker 沙箱中直接编辑源文件补全实现，然后通过文件完整性检查、AST 归一化校验和编译+单元测试验证结果的正确性。

**数据集规模**: 539 个 task（305 个 `.c` + 234 个 `.h`，均通过 tree-sitter AST 验证确认有测试断言）

***

## 架构总览

```
┌──────────────────────────────────────────────────────────────────────────────┐
│  Host (WSL2 Ubuntu)                                                          │
│                                                                              │
│  ┌───────────┐      ┌──────────────────────────┐      ┌──────────────────┐   │
│  │ runner.py │─────▶│    agent.py               │─────▶│  Claude 容器      │   │
│  │ CLI 入口   │      │    ClaudeAgent.run()      │      │  (docker run)     │   │
│  └───────────┘      │                          │      │                   │   │
│                     │  准备 (Steps 1–4):        │      │  --cap-drop ALL   │   │
│  ┌───────────┐      │    1. hard_reset_repo     │      │  no-new-privileges│   │
│  │ mask.py    │◀─────│    2. _prepare_workspace │      │  --pids-limit 256 │   │
│  │ tree-sitter│      │    3. _capture_snapshot  │      │  --memory 4g      │   │
│  │ AST 操作   │      │    4. _snapshot_files    │      │  --cpus 2         │   │
│  └───────────┘      │                          │      │                   │   │
│                     │  执行 (Step 5):           │      │  编辑目标源文件     │   │
│  ┌───────────┐      │    5. _run_claude()      │      │  搜索上下文         │   │
│  │ git_utils  │      │                          │      │  输出完成标记       │   │
│  │ git reset  │      │  校验 (Steps 6–9):       │      └──────────────────┘   │
│  │ git clean  │      │    6. 文件完整性检查      │                             │
│  └───────────┘      │    7. 读 Claude 最终源码  │     ┌──────────────────┐   │
│                     │    8. 生成 unified diff   │     │  验证容器          │   │
│  ┌───────────┐      │    9. AST 归一化+关键字   │     │  (docker run)     │   │
│  │ logger.py  │      │                          │     │                   │   │
│  │ 轨迹日志   │      │  测试 (Step 10):         │     │  --network none   │   │
│  └───────────┘      │   10. _verify_tests()    │     │  编译 + 运行测试   │   │
│                     │                          │     └──────────────────┘   │
│                     │  收尾 (Steps 11–13):      │                            │
│                     │   11. _record_trajectory │                            │
│                     │   12. _make_result       │                            │
│                     │   13. cleanup + verify   │                            │
│                     └──────────────────────────┘                            │
└──────────────────────────────────────────────────────────────────────────────┘
```

**核心设计原则**: Claude 是纯黑盒 Agent，不运行测试、不编译代码，只负责阅读源码上下文并编辑目标文件。测试验证完全由 Python 框架在独立的断网 Docker 容器中完成。

***

## 文件详细说明

### `runner.py` — CLI 入口与任务调度

**参数**:

| 参数 | 说明 |
| --- | --- |
| `--check` | 验证所有外部依赖（Docker、镜像、grammar、settings、数据集）后退出 |
| `--task-id ID` | 运行单个 task |
| `--batch N` | 批量运行前 N 个 task |
| `--start ID` | 从指定 task_id 开始（配合 `--batch` 使用，直观跳过不需要的条目） |
| `--resume` | 跳过 `results.jsonl` 中已通过的 task（与 `--task-id` / `--batch` 均可搭配） |
| `--data FILE [FILE ...]` | 指定数据集文件（默认使用经 tree-sitter 验证的 `.verified.jsonl`） |

**关键函数**:

| 函数 | 作用 |
| --- | --- |
| `load_task_by_id(task_id, task_files)` | 遍历 JSONL 文件，返回第一个匹配 task_id 的对象 |
| `load_finished_tasks()` | 读取 `results.jsonl`，返回所有已通过 task_id 集合 |
| `append_result(result)` | 追加一行 JSON 到 `results/results.jsonl` |
| `run_task(agent, task)` | 执行 agent.run() 并打印 PASS/FAIL |
| `_batch_main(task_files, agent, n, resume, start_task_id)` | 批量加载并执行；resume 时跳过已通过 task；start_task_id 指定起始 task_id |
| `run_checks()` | 检查 Docker / 镜像 / grammar / settings / 数据集是否就绪 |

***

### `agent.py` — 核心 Pipeline

**类**: `ClaudeAgent(repo_root)` — 保存 RIOT 根目录路径。

**主入口**: `run(task)` — 完整 13 步流程：

```
run(task)
  │
  ├─ 1. hard_reset_repo(repo_root, clean=True)
  │      git reset --hard HEAD + git clean -fd
  │      排除 riot-claude/ 和 .claude_home/
  │
  ├─ 2. workspace_dir = _prepare_workspace(task)
  │      创建 ~/.riot-workspaces/task_{id}/
  │      ├─ repo/          ← RIOT 源码副本（shutil.copytree，不含 .git）
  │      └─ claude_home/   ← Claude settings.json
  │      → 删除 tests/ docs/ examples/ samples/
  │      → tree-sitter mask 目标函数体
  │      → 权限锁定：目录 555，文件 a-w，目标 644
  │
  ├─ 3. snapshot = _capture_snapshot(task, repo_dir)
  │     git HEAD, docker digest, model, prompt SHA256, env, Makefile hash
  │
  ├─ 4. fs_before = _snapshot_files(repo_dir)
  │     find 记录所有文件的 (路径, 大小, mtime)
  │
  ├─ 5. claude_completed, output = _run_claude(task, workspace_dir)
  │     Docker 容器 → stdin pipe 传 prompt → 实时监视 stdout
  │
  ├─ 6. fs_after = _snapshot_files(repo_dir)
  │     _check_file_integrity(before, after, target_path)
  │
  ├─ 7. final_source = ws_file.read_text()
  │
  ├─ 8. diff = unified_diff(masked, final)
  │
  ├─ 9. AST 完整性校验
  │     check_ast_integrity() → 归一化对比
  │     check_body_integrity() → 关键字检查 #define 等
  │
  ├─ 10. _verify_tests(task, workspace_dir)
  │       独立容器（--network none）编译 + 运行测试
  │
  ├─ 11. _record_trajectory(task, output, diff, illegal)
  │
  ├─ 12. return _make_result(...)
  │
  └─ 13. finally: cleanup + verify
          shutil.rmtree() → Docker root 回退 → _verify_cleanup()
          → hard_reset_repo(clean=True)
```

#### Docker 容器配置

**Claude 容器** (`_run_claude`):

```bash
docker run --rm -i \
  --cap-drop ALL \
  --security-opt no-new-privileges \
  --pids-limit 256 --memory 4g --cpus 2 \
  --user $(id -u):$(id -g) \
  -e ANTHROPIC_API_KEY=... -e ANTHROPIC_BASE_URL=... \
  -e BUILD_DIR=/tmp/build -e BINDIRBASE=/tmp/build/bin \
  -v {repo_dir}:/workspace:rw \
  -v {claude_home}:/home/agent/.claude:rw \
  --tmpfs /tmp:size=1g,exec \
  --tmpfs /home/agent/.cache:size=500m \
  --tmpfs /home/agent/.config:size=100m \
  -w /workspace \
  --entrypoint bash \
  riot-sandbox:latest \
  -c 'claude --dangerously-skip-permissions -p "$(cat)"'
```

关键细节：
- `--tmpfs /tmp:...exec` — 需要 `exec` 标志以运行编译产物
- `-p "$(cat)"` — 从 stdin 读取 prompt（避免 shell 转义问题）
- `claude-on-completion` hook 输出 `=== RIOT_TASK_COMPLETED ===` 标记进程完成
- 环境变量从 Windows 侧 `settings.json` 的 `env` 块读取（Claude Code 2.1.146+ 忽略 settings 中的 `env` 块）
- 环境变量白名单：`ANTHROPIC_API_KEY`、`ANTHROPIC_AUTH_TOKEN`、`ANTHROPIC_BASE_URL`

**验证容器** (`_verify_tests`):

```bash
docker run --rm -i \
  --network none \
  --cap-drop ALL --security-opt no-new-privileges \
  --pids-limit 256 --memory 4g --cpus 2 \
  --user $(id -u):$(id -g) \
  -e BUILD_DIR=/tmp/build -e BINDIRBASE=/tmp/build/bin \
  -v {repo_dir}:/workspace:rw \
  -v {original_tests}:/riot_tests:ro \
  --tmpfs /tmp:size=1g,exec \
  -w /workspace \
  --entrypoint bash \
  riot-sandbox:latest \
  -c 'cp -r /riot_tests /workspace/tests && cd /workspace \
      && make -C tests/unittests BOARD=native64 clean \
      && {run_command}'
```

关键细节：
- `--network none` — 完全断网
- tests 先挂载到 `/riot_tests:ro` 再 `cp -r` 到 `/workspace/tests`——不能用符号链接，否则 Makefile.include 的相对路径解析会断裂
- 判断逻辑：编译失败 → 崩溃 → 目标测试失败，逐级递进
- 只关心 `unit_test` 字段指定的测试用例

#### 工作区锁定

```
所有目录   chmod 555    → 不能创建/删除/重命名
所有文件   chmod a-w    → 不可写（保留可执行位）
目标文件   chmod 644    → 唯一可写
tests/     删除         → Agent 看不到测试断言
docs/ examples/ samples/   → 删除，防止参考实现泄露
```

#### 清理验证

`_verify_cleanup()` 检查三项：工作区目录是否已删除、Docker 容器是否残留、git 仓库是否干净。任何失败记录到 `cleanup_errors` 并打印 `[CLEANUP WARNING]`。工作区删除采用两级回退：先 `shutil.rmtree()`，若权限不足则用 Docker root 容器 `chmod -R a+rwX && rm -rf`。

***

### `mask.py` — tree-sitter C 函数 AST 操作

基于 tree-sitter 的 C 语言 AST 分析。解析器为全局单例，复用 `/riot-bench/build/my-languages.so` 编译产物。

**核心函数**:

| 函数 | 作用 |
| --- | --- |
| `apply_mask(path, fn_name, masked_code)` | 找到函数定义 → 替换函数体为 `/* MASKED */`；找不到 body 则用 masked_code 整体替换 |
| `extract_function_body(source, fn_name)` | 提取函数体内容（不含大括号） |
| `normalize_except_body(source, fn_name)` | 函数体 → 占位符，参数名 → `p0, p1, ...` |
| `check_ast_integrity(masked, final, fn_name)` | 归一化比较：一致 = 只改了函数体 |
| `check_body_integrity(body)` | 关键字检查：`#define` `#undef` `#include` `typedef` |

设计要点：
- 参数名泛型化防止重命名误杀
- AST 解析失败时 fail-open（不阻塞 RIOT 中非常规宏的任务）

***

### `config.py` — 全局配置

| 变量 | 默认值 | 说明 |
| --- | --- | --- |
| `RIOT_ROOT` | `riot-claude/../` | RIOT 仓库根目录 |
| `BENCH_ROOT` | `riot-claude/../../riot-bench/` | tree-sitter 编译产物目录 |
| `TEST_TIMEOUT` | 300s | 验证容器超时 |
| `WATCHDOG_TIMEOUT` | 28800s (8h) | 安全保险丝（仅异常情况触发） |
| `MAX_OUTPUT_LENGTH` | 2,000,000 | 轨迹日志截断阈值 |
| `DOCKER_IMAGE` | `riot-sandbox:latest` | Docker 镜像 |
| `WORKSPACE_BASE` | `~/.riot-workspaces` | 工作区根目录（可通过 `RIOT_WORKSPACE_BASE` 环境变量覆盖） |
| `CLAUDE_SETTINGS_SRC` | 自动扫描 `/mnt/c/Users/*/.claude/settings.json` | 宿主机 Claude 配置 |
| `CLAUDE_NETWORK_MODE` | `host` | Claude 容器网络模式 |
| `DOCKER_MEMORY` / `DOCKER_CPUS` / `DOCKER_PIDS_LIMIT` | `4g` / `2` / `256` | 容器资源限制 |

***

### `prompt.py` — Agent Prompt 模板

发给 Claude 的任务描述，包含 `{sut_function}` 和 `{source_path}` 占位符。通过 `str.format()` 填充后经 stdin pipe 传入容器。每次运行的 prompt 计算 SHA256 存入快照。

***

### `git_utils.py` — Git 仓库重置

**`hard_reset_repo(repo_root, clean=False)`**:

```bash
git reset --hard HEAD
git clean -fd -e "riot-claude" -e ".claude_home"
```

整个 `riot-claude/` 目录和 `.claude_home/` 被排除，不会被 clean 删除。在 `run()` 的开头和 finally 块各调用一次。

***

### `logger.py` — 轨迹日志

`append_trajectory(path, content)` — 追加写入 UTF-8 内容到轨迹文件，自动创建父目录。

***

### `Dockerfile` — 沙箱镜像

基于 `ubuntu:24.04`：

| 组件 | 用途 |
| --- | --- |
| `build-essential` | 基础编译工具链 |
| `gcc-13-multilib` / `g++-13-multilib` | 32/64 位交叉编译 |
| `make` / `cmake` / `python3` | 构建系统 |
| `python3-pexpect` | RIOT 测试框架依赖 |
| `Node.js 22.x` | Claude Code CLI 运行环境 |
| `@anthropic-ai/claude-code@2.1.146` | Claude Code CLI（npm 全局安装） |

非 root 用户 `agent`，WORKDIR `/workspace`。

***

### 数据集文件

**`riot_tasks_with_commands.c.verified.jsonl`** (305 tasks) + **`riot_tasks_with_commands.h.verified.jsonl`** (234 tasks)

这两个是经 `riot-bench/verify_tasks.py`（tree-sitter AST 分析）验证后的数据集，只包含测试中确实有 `TEST_ASSERT*` 断言的条目。原始未过滤数据在 `riot-bench/` 目录下。

数据集处理脚本位于 `riot-bench/`：
- `build_dataset.py` — 从 RIOT 源码提取函数、生成 task
- `verify_tasks.py` — tree-sitter AST 验证 sut_function ↔ test 映射，输出 verified JSONL 到 `riot-claude/`
- `extract_unit_tests.py` — 提取 RIOT 仓库中所有测试用例
- `mask_engine.py` — 函数体 mask 工具

每行一个 JSON：

```json
{
  "task_id": "1",
  "source_path": "sys/analog_util/adc_util.c",
  "sut_function": "adc_util_map",
  "oracle": "oracles/1.c",
  "masked_code": "int32_t adc_util_map(int sample, adc_res_t res, int32_t min, int32_t max)\n{ ... }",
  "unit_test": "test_adc_util_map",
  "run_command": "make -C tests/unittests BOARD=native64 tests-analog_util ..."
}
```

| 字段 | 说明 |
| --- | --- |
| `task_id` | 唯一标识符 |
| `source_path` | 相对 RIOT 根目录的源文件路径 |
| `sut_function` | 待补全函数名 |
| `oracle` | oracle 参考实现（框架当前未使用） |
| `masked_code` | 原始函数签名（mask 失败时的回退替换） |
| `unit_test` | 对应的单元测试名 |
| `run_command` | 验证容器中执行的编译+测试命令 |

***

## Pipeline 完整步骤

```
1. 仓库清理       hard_reset_repo(clean=True)
2. 工作区准备      copytree → 删 tests/docs/examples/samples → mask →
                   权限锁定
3. 环境快照        git HEAD, docker digest, model, prompt SHA256
4. 文件快照        find 记录基准 (路径, 大小, mtime)
5. 执行 Claude     Docker 容器, stdin pipe 传 prompt, 实时监视
6. 文件完整性      对比前后快照
7. 读取最终源码
8. 生成 Diff       difflib.unified_diff
9. AST 校验        归一化 + 关键字检查
10. 编译与测试      独立断网容器编译 + 运行
11. 记录轨迹       写入 trajectory/task_{id}.log
12. 返回结果       构造结构化 JSON
13. 清理与验证      rmtree → Docker root 回退 → verify_cleanup → git reset
```

## 安全机制

### 三层防作弊

| 层 | 机制 | 防止 |
| --- | --- | --- |
| 工作区锁定 | 目录 555 + 文件 a-w + 目标 644 | 修改非目标文件 |
| 文件完整性检查 | 前后快照对比 | 创建/删除/修改其他文件 |
| AST 校验 | 归一化 + 关键字检查 | 函数体外修改 + 预处理指令作弊 |

### 容器隔离

Claude 容器：`--cap-drop ALL`、`no-new-privileges`、`--user` 非 root、`--pids-limit 256`、`--memory 4g`、`--cpus 2`、工作区无 `.git`。

验证容器：同上 + `--network none`（完全断网）。

### 清理验证

每次 task 结束后验证三项：工作区目录已删除、Docker 容器无残留、git 仓库干净。失败打印 `[CLEANUP WARNING]` 并写入结果。

***

## CLI 使用

必须在 WSL 内执行：

```bash
# 依赖检查
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --check"

# 单个 task
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --task-id 1"

# 批量运行
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --batch 10"

# 指定数据集
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --batch 10 --data riot_tasks_with_commands.c.verified.jsonl"

# 从指定 task_id 开始（包含task_id）
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --batch 58 --start 112"

# 断点续跑
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --batch 20 --resume"

# 全量
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --batch 539"
```

***

## 输出文件

### `results/results.jsonl`

每行一个 JSON：

```json
{
  "task_id": "1",
  "passed": true,
  "claude_completed": true,
  "runtime_s": 77.2,
  "illegal_changes": [],
  "file_tampering": [],
  "error": null,
  "error_category": "none",
  "total_test_failures": 0,
  "patch": "--- sys/...\n+++ sys/...\n...",
  "snapshot": {
    "git_head": "abc123...",
    "docker_image_digest": "riot-sandbox@sha256:def456...",
    "claude_model": "claude-sonnet-4-6",
    "prompt_sha256": "e3b0c442...",
    "claude_env_vars": "-e ANTHROPIC_API_KEY=...",
    "Makefile_sha256": "..."
  }
}
```

`error_category` 可选值：`none` / `illegal_modifications` / `file_tampering` / `compile_error` / `crash` / `test_failure` / `test_not_executed` / `timeout` / `watchdog` / `workspace_setup` / `exception` / `api_error`

### `trajectory/task_{id}.log`

按阶段追加的完整执行日志：Claude session 输出、diff、违规记录、编译+测试输出。超长输出截断为头尾各半。

***

## Docker 镜像构建

```bash
wsl bash -c "cd /home/carl/RIOT/riot-claude && docker build -t riot-sandbox:latest ."
```

***

## 运行环境

- **WSL2** (Ubuntu)
- **Docker Desktop** for Windows（WSL2 backend）
- **Python 3.10+** + `tree-sitter`（WSL 内）
- **`my-languages.so`** tree-sitter C grammar（在 `riot-bench/build/` 下）
- **Claude Code CLI**（Docker 镜像内）
- **Claude API Key**（Windows 侧 `%USERPROFILE%\.claude\settings.json`）
