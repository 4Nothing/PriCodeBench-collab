# QSem-Claude

Claude Code 驱动的 QSemOS 函数补全 benchmark 框架。

## 目录结构

```
Private_Code/
├── PrivateAPIEval_agent.jsonl      ← 数据集（agent 不可见）
├── oracles/                        ← func_under_test.c 黄金备份（agent 不可见）
└── QSemOS/                         ← agent cwd，只能看到这里
    ├── src/                        ← 源码，agent 可读
    ├── check_tests/                ← 测试用例，跑前隐藏
    │   ├── common/
    │   ├── include/
    │   └── tasks/
    │       ├── task_001/
    │       │   ├── func_under_test.c  ← 被测函数（ground truth）
    │       │   ├── test_*.c           ← 测试用例 + 断言
    │       │   ├── Makefile
    │       │   └── auto_stub.h
    │       └── ... (240 tasks)
    └── qsem-claude/                ← benchmark 框架
        ├── config.py
        ├── agent.py
        ├── runner.py
        ├── mask.py
        ├── prompt.py
        ├── logger.py
        ├── analyze_results.py
        ├── git_utils.py
        ├── results/       ← 结构化结果 JSONL
        └── trajectory/    ← 每个 task 的完整日志
```

Agent 能看到：`src/`  
Agent 看不到：`check_tests/`（被 mv） `oracles/` `PrivateAPIEval_agent.jsonl`（在 QSemOS 外）

## 依赖

- Python 3.11+ + conda
- `pip install tree-sitter tree-sitter-c`
- Claude Code CLI（需登录）

## 使用方法

```bash
cd Private_Code/QSemOS/qsem-claude

# 检查依赖
conda run -n base python runner.py --check

# 运行单个 task
conda run -n base python runner.py --task-id 1

# 批量运行前 N 个 task
conda run -n base python runner.py --batch 10

# 断点续跑（跳过已通过的 task）
conda run -n base python runner.py --batch 240 --resume

# 从指定 task 开始
conda run -n base python runner.py --batch 100 --start 50

# 分析结果
conda run -n base python analyze_results.py
```

## 数据集格式

`PrivateAPIEval_agent.jsonl`，每行一个 task：

```json
{
  "task_id": "10",
  "source_path": "src/core/kernel/task/smp/prt_task_priority.c",
  "sut_function": "OsSemPrioBListLock",
  "masked_source": "U32 OsSemPrioBListLock(struct TagTskCb*)\n{ /* MASKED */ }",
  "unit_test": "test_OsSemPrioBListLock",
  "run_command": "cd QSemOS/check_tests/tasks/task_010 && make && ./test"
}
```

| 字段 | 说明 |
|------|------|
| `task_id` | 对应 `check_tests/tasks/task_NNN/` |
| `source_path` | agent 编辑的目标文件（QSemOS/src/ 下） |
| `sut_function` | 要实现的函数名 |
| `masked_source` | mask 后的函数签名（函数体 = `/* MASKED */`） |
| `unit_test` | 测试函数名 |
| `run_command` | 编译+运行测试的 shell 命令 |

## 单个 task 执行流程（12 步）

以 `task_id=1, sut_function=OsPerfDefaultNotify` 为例。

### 步骤 1：备份源文件

```python
backup_source = QSemOS/src/extended/perf/prt_perf_output.c 的完整内容
```

存到内存变量。

### 步骤 2：备份 func_under_test.c

```python
backup_func = QSemOS/check_tests/tasks/task_001/func_under_test.c 的完整内容
```

即 ground truth（此处用占位符代替真实答案）：

```c
static void OsPerfDefaultNotify(void)
{
    /* GROUND TRUTH - hidden from agent */
}
```

### 步骤 3：锁定权限

```bash
find QSemOS/src/ -type d -exec chmod 555 {} +   # 目录 r-xr-xr-x（不可增删条目）
find QSemOS/src/ -type f -exec chmod a-w {} +   # 文件 r--r--r--（全部只读）
chmod u+w QSemOS/src/extended/perf/prt_perf_output.c  # 目标 rw-r--r--
```

| 操作 | 目标文件 | 其他文件 | 目录 |
|------|:---:|:---:|:---:|
| 读 | ✓ | ✓ | ✓ |
| 编辑 | ✓ | ✗ | — |
| 删除/重命名/新建 | ✗ | ✗ | ✗ |

### 步骤 4：Mask 目标函数体

tree-sitter 解析 AST → 定位 `OsPerfDefaultNotify` → 替换 body → 写回文件：

```c
// 改前（ground truth — 占位符，非真实答案）
static void OsPerfDefaultNotify(void)
{
    /* GROUND TRUTH - hidden from agent */
}

// 改后（Claude 看到的）
static void OsPerfDefaultNotify(void)
{
    /* MASKED - implement this function */
}
```

只替换函数体，函数签名、属性宏、文件其余内容完全不动。

### 步骤 5：环境快照

```python
snapshot = {
    "timestamp":       "2026-06-17T08:30:00Z",
    "claude_model":    "claude-opus-4-7",
    "claude_code_version": "Claude Code v2.1.xxx",
    "prompt_sha256":   "abc123...",
    "prompt_text":     "Implement the missing...",
    "task":            {...},
    "dataset_sha256":  "def456...",
    "tree_sitter_c_version": "0.24.2",
    "python_version":  "3.13.x"
}
```

### 步骤 6：隐藏 check_tests，启动 Claude

```bash
mv QSemOS/check_tests → QSemOS/.check_tests_hidden
```

stdin pipe 启动 Claude Code CLI（cwd=QSemOS/）：

```
Implement the missing function `OsPerfDefaultNotify` in `src/extended/perf/prt_perf_output.c`.

The function body is currently replaced with a placeholder comment. Read the surrounding
code and header files to understand the expected behavior, then complete the implementation.

RULES:
- Only edit the file src/extended/perf/prt_perf_output.c
- Do NOT modify any other files
- Do NOT delete or modify any existing code outside the target function body
```

框架通过 `selectors` 实时读取子进程 stdout：
- 打印 Claude 输出（去 ANSI 色码）
- 检测 hook 标记 `=== QSEM_TASK_COMPLETED ===`
- watchdog：8 小时无输出 → kill 进程

### 步骤 7：恢复 check_tests 和权限

```bash
mv QSemOS/.check_tests_hidden → QSemOS/check_tests
find QSemOS/src/ -type d -exec chmod 755 {} +
find QSemOS/src/ -type f -exec chmod u+w {} +
```

放在 `finally` 块中，前面任何一步抛异常都会执行。

### 步骤 8：归一化等价性检查

对 masked 版和 Claude 编辑后的版本分别归一化：

```
1. 函数体 → { /* BENCHMARK BODY */ } 占位符
2. 参数名 → p0, p1, p2...
3. 签名空白归一化（连续空白 → 单空格）

masked_norm == final_norm → 只有目标函数体被修改 ✓
masked_norm != final_norm → 函数外代码被修改 ✗
```

记录到 `illegal` 列表但**不阻断**后续流程。

### 步骤 9：提取函数体，注入 func_under_test.c

```python
body = extract_function_body(编辑后的源码, "OsPerfDefaultNotify")
_write_func_body(func_under_test.c, body)
```

tree-sitter 从编辑后的源码提取函数体 → 定位 func_under_test.c 中的 body → 替换 ground truth 为 agent 实现。

### 步骤 10：编译运行测试

```
cwd = Private_Code/
执行: cd QSemOS/check_tests/tasks/task_001 && make && ./test
```

测试程序 `#include "func_under_test.c"`，调用被测函数，比较输出和预期。

输出解析：

| 检测模式 | 判定 | error_category |
|---------|------|---------------|
| `undefined reference to` `error:` `make: ***` | 编译失败 | `compile_error` |
| `Segmentation fault` `Aborted` `panic` `signal` | 运行时崩溃 | `crash` |
| `.test_OsPerfDefaultNotify (...) exp X was Y` | 目标测试断言失败 | `test_failure` |
| 无 embUnit 输出或 shell 报错 | 测试未执行 | `test_not_executed` |
| `Pass-Rate: 100.00%` 或 `OK (... tests)` | 全部通过 | `none` |

### 步骤 11：记录结果

`results/results.jsonl` 追加一行：

```json
{
  "task_id": "1",
  "passed": true,
  "claude_completed": true,
  "runtime_s": 45.2,
  "illegal_changes": [],
  "file_tampering": [],
  "error": null,
  "error_category": "none",
  "patch": "@@ -1,4 +1,4 @@...",
  "snapshot": {
    "timestamp": "2026-06-17T08:30:00Z",
    "claude_model": "claude-opus-4-7",
    "claude_code_version": "Claude Code v2.1.xxx",
    "prompt_sha256": "abc123...",
    "prompt_text": "Implement the missing function...",
    "task": {"task_id": "1", "source_path": "...", ...},
    "dataset_sha256": "def456...",
    "tree_sitter_c_version": "0.24.2",
    "python_version": "3.13.x"
  },
  "total_test_failures": 0
}
```

`trajectory/task_1.log` 写入完整轨迹：

```
============================================================
=== Claude Session ===        ← Claude 完整 stdout（超长截断）
============================================================
=== Diff ===                  ← unified diff
============================================================
=== ILLEGAL MODIFICATIONS === ← 违规信息（如有）
============================================================
=== Test Output ===           ← 测试 stdout + stderr
```

### 步骤 12：恢复文件

```python
# finally 块保证执行
prt_perf_output.c  ← backup_source
func_under_test.c  ← backup_func   # 即 ground truth
```

QSemOS 恢复到 task 执行前的干净状态，下一个 task 安全开始。

## 异常保护

| 场景 | 记录结果 | 恢复文件 | 恢复权限 |
|------|:---:|:---:|:---:|
| Claude 正常完成 | ✓ | ✓ | ✓ |
| watchdog kill（8h 无输出） | ✓ | ✓ | ✓ |
| 函数体提取失败（body=None） | ✓ | ✓ | ✓ |
| 测试超时（120s） | ✓ | ✓ | ✓ |
| Python 异常崩溃 | ✓ | ✓ | ✓ |
| kill -9 / 断电 | ✗ | ✗ | 手动 `find src/ -exec chmod u+w {} +` |

## 结果指标

批量跑完后汇总输出：

```
==================================================
SUMMARY (240 tasks)
==================================================
  Test pass rate:        200/240 (83.3%)     ← 指标一：测试通过率
  Clean pass:            185/240 (77.1%)     ← 完整性 OK + 测试通过
  Dirty pass:             15/240 ( 6.3%)     ← 完整性违规 + 测试通过
  Dirty fail:              5/240 ( 2.1%)     ← 完整性违规 + 测试失败
  ---
  Integrity OK rate:     220/240 (91.7%)     ← 只改了目标函数体的比例

  task 1: PASS (45s)
  task 5: PASS [DIRTY] (62s)           ← [DIRTY] 标记表示有违规
  task 7: FAIL [DIRTY] [compile_error] (30s)
```

四个分类：

| | 测试通过 | 测试失败 |
|---|---|---|
| **完整性 OK** | Clean pass | Clean fail |
| **完整性违规** | Dirty pass | Dirty fail |

## 复现

拿到 `results.jsonl` 中任意一行的 `snapshot` 可完全复现：

1. 确认 `dataset_sha256` 一致
2. 使用相同 `claude_model`
3. 使用相同 `prompt_text`
4. 确认 `claude_code_version` 一致
5. 确认 `tree_sitter_c_version` `python_version` 一致

## 与 riot-claude 的区别

| | riot-claude | qsem-claude |
|---|---|---|
| 被评项目 | RIOT OS | QSemOS |
| 位置 | `RIOT/riot-claude/` | `QSemOS/qsem-claude/` |
| 执行环境 | Docker 容器 | 本地 claude CLI |
| 工作区 | 复制到 tmp | 直接操作 QSemOS 原文件 |
| 版本管理 | git reset --hard | 文件备份恢复 |
| Agent 视野 | workspace 副本 | QSemOS/ 目录 |
| 答案隔离 | 删除 tests/ | mv check_tests + oracles 在 QSemOS 外 |
| 权限控制 | chmod + Docker 隔离 | chmod |
| mask 语法 | riot-bench .so 文件 | tree_sitter_c Python 包 |
| 完整性检查 | 仅归一化（无关键字检查） | 仅归一化 |
| 违规处理 | 不阻断测试 | 不阻断测试 |
| Clean/Dirty 分类 | ✓ | ✓ |
| 快照 | git/Docker/prompt hash | model/CLI/prompt/dataset hash |
| 代码量 | ~2100 行 | ~1150 行 |
