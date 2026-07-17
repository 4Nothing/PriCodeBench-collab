# riot-bench: 数据集构建与验证工具

从 RIOT-OS 源码中提取函数定义、生成补全任务、验证测试可靠性，是 PriCodeBench 的数据管线。

---

## 概览

```
RIOT 源码 (sys/ core/ drivers/ cpu/)
        │
        ▼
extract_unit_tests.py      ← 扫描测试注册，提取 unit_tests.jsonl
        │
        ▼
build_dataset.py            ← tree-sitter 匹配函数定义 → 生成 task + oracle
        │
        ▼
verify_tasks.py             ← AST 验证 sut_function 是否真的被 TEST_ASSERT 断言
        │
        ▼
riot-claude/*.verified.jsonl  ← 最终数据集，喂给 runner.py
```

一共四个脚本，前三个是数据管线，最后一个用来验证数据管线自身的正确性。

---

## 文件说明

| 文件 | 作用 |
|---|---|
| `extract_unit_tests.py` | 用 ripgrep 扫描 `tests/unittests/`，从 `new_TestFixture(test_xxx)` 注册中提取所有单元测试，输出 `unit_tests.jsonl`（1243 条） |
| `build_dataset.py` | 用 tree-sitter 解析测试文件中的函数调用，在 RIOT 源码目录中查找定义，按优先级（sys > drivers > core > cpu）选择最佳实现，mask 函数体生成 task。输出 `riot_tasks_with_commands.{c,h}.jsonl` + `oracles/` |
| `verify_tasks.py` | 用 tree-sitter AST 验证每个 task 的 `sut_function` 是否真的被 `TEST_ASSERT*` 断言覆盖。三种模式：直接断言、变量追踪、邻行副作用。过滤掉"只是被调用但没被断言"的函数。输出 `.verified.jsonl` |
| `verify_unit_tests.py` | 编译 + 运行所有 `run_command`，确认测试能正常通过，输出 `verify_results.json` + `verify_logs/` |
| `mask_engine.py` | 简易大括号匹配 mask 工具 —— 保留签名，函数体替换为 `/* TODO(agent) */` |
| `build/my-languages.so` | tree-sitter C 语言 parser 编译产物，被 `build_dataset.py`、`verify_tasks.py`、`riot-claude/mask.py` 共享 |

---

## 数据集统计

| 阶段 | 产出 | 数量 |
|---|---|---|
| 原始 task（`build_dataset.py`） | `riot_tasks_with_commands.c.jsonl`<br>`riot_tasks_with_commands.h.jsonl` | 390 (.c) + 250 (.h) = **640** |
| 验证后 task（`verify_tasks.py`） | `*.verified.jsonl` | 305 (.c) + 234 (.h) = **539** |

验证过滤掉了 101 个 task（105 条被调用但未被断言 + 部分无法定位测试源文件），确保数据集中每个 task 的 `unit_test` 确实有 `TEST_ASSERT*` 验证目标函数。

---

## Task 格式

```json
{
  "task_id": "1",
  "source_path": "sys/checksum/crc8.c",
  "sut_function": "crc8",
  "oracle": "oracles/1.c",
  "masked_code": "uint8_t crc8(const uint8_t *data, size_t len, uint8_t g_polynom, uint8_t crc)\n{\n    /* TODO(agent) */\n}",
  "unit_test": "test_checksum_crc8_sequence_1a",
  "run_command": "make -C tests/unittests BOARD=native64 tests-checksum ..."
}
```

| 字段 | 说明 |
|---|---|
| `task_id` | 任务编号（.c / .h 各自独立编号） |
| `source_path` | 函数所在源文件（相对 RIOT 根目录） |
| `sut_function` | 待补全的函数名 |
| `oracle` | oracle 文件路径（完整实现，框架当前未使用） |
| `masked_code` | mask 回退替换（tree-sitter mask 失败时使用） |
| `unit_test` | 验证用的单元测试函数名 |
| `run_command` | 编译 + 测试命令 |

---

## 使用方法

```bash
cd riot-bench

# 1. 提取单元测试（在 riot-bench/ 目录执行）
python3 extract_unit_tests.py ../RIOT

# 2. 构建补全任务
python3 build_dataset.py --repo ../RIOT --out-prefix riot_tasks_with_commands

# 3. AST 验证（过滤无断言的 task，输出到 ../RIOT/riot-claude/）
python3 verify_tasks.py riot_tasks_with_commands.c.jsonl
python3 verify_tasks.py riot_tasks_with_commands.h.jsonl

# 4. 验证所有测试可正常通过
python3 verify_unit_tests.py --riot-root ../RIOT
```

---

## 设计细节

- **函数定义优先级**：`sys/` (5) > `drivers/` (4) > `core/` (3) > `cpu/` (2)。同一个函数可能在多处声明（头文件中的原型 vs 源文件中的定义），优先取源文件实现
- **tree-sitter 偏移修复**：`static inline` 函数有时丢失首字节，代码通过向前回溯 `[a-zA-Z0-9_]` 字符修复
- **全大写过滤**：自动跳过宏（全大写的函数名），避免 `#define` 被误当函数
- **三种断言检测**：直接嵌套 (`TEST_ASSERT(foo())`)、变量追踪 (`int x = foo(); TEST_ASSERT(x)`)、邻行副作用 (`foo(ptr); TEST_ASSERT(*ptr)`)
- **排除目录**：`tests/` `build/` `dist/` `bin/` `.git/`，确保只从源码目录提取

---

## 依赖

- Python 3.8+
- `tree-sitter`
- ripgrep (`rg`)
- RIOT 构建工具链 (make, gcc, gcc-multilib)
