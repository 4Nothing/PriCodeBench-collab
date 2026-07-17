# PriCodeBench: AI 嵌入式代码补全能力基准测试

两个基准测试套件，评估 AI Agent 在真实嵌入式操作系统源码中补全 C 函数的能力——一个开源（RIOT）一个闭源（QSemOS）。

---

## 仓库结构

```
research/
├── Open_source/                      ← 开源基准测试
│   ├── RIOT/                         ← RIOT-OS 实时操作系统源码
│   │   └── riot-claude/              ← 基准测试框架（runner / agent / mask / Docker）
│   ├── riot-bench/                   ← 数据集构建 & 验证工具
│   └── README_NEW.md
│
├── Closed_source/                    ← 闭源基准测试
│   ├── QSemOS/                       ← QSemOS 闭源嵌入式 OS
│   │   └── qsem-claude/              ← 基准测试框架
│   ├── PrivateAPIEval_agent.jsonl    ← 私有 API 评估数据
│   ├── oracles/                      ← oracle 参考实现
│   └── README_NEW.md
│
└── README_NEW.md                     ← 本文件
```

---

## Open_source: RIOT

从 RIOT 实时操作系统中选取 538 个真实 C 函数，AI Agent 在 Docker 沙箱中补全被挖空的函数体，编译 + 单元测试验证。

| 指标 | 数值 |
|---|---|
| 任务数 | 538 (305 .c + 234 .h) |
| 开源 | 全部源码可见 |
| 隔离 | Docker 容器 (--cap-drop ALL, --network none) |
| 防作弊 | 权限锁定 + SHA256 快照 + AST 归一化 |

详细说明 → `Open_source/README_NEW.md`

---

## Closed_source: QSemOS

240 个闭源 OS 内核函数，覆盖任务调度、中断、IPC、同步、定时器、性能监控、动态模块加载。

| 指标 | 数值 |
|---|---|
| 任务数 | 240 |
| API 文档 | 不可见（头文件需推断） |
| 隔离 | chmod + tree-sitter mask |
| 独有模块 | spinlock, signal, rwlock, dlmodule, perf |

详细说明 → `Closed_source/README_NEW.md`

---

## 结果对比

| 模型 | RIOT (开源) | QSemOS (闭源) |
|---|---|---|
| DeepSeek V4 Pro | **90.7%** (488/538) | **82.9%** (199/240) |
| GLM 5.1 | — | **80.8%** (194/240) |

闭源下头文件和 API 不可见，通过率下降约 8 个百分点。两份 benchmark 互补：RIOT 测试通用嵌入式编程能力，QSemOS 测试黑盒推理和同步原语理解。
