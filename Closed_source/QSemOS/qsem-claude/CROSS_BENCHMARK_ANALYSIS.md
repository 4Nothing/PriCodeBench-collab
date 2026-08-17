# QSemOS × RIOT 跨基准对比分析

**模型**: DeepSeek-v4-pro
**QSemOS**: 闭源嵌入式 OS kernel，237 tasks（剔除 3 个上下文不可获取任务: task 1, 5, 133）
**模型**: DeepSeek-v4-pro（RIOT + QSemOS）、GLM-5.1（QSemOS + RIOT）

***

## 1. 宏观对比

| 指标    | RIOT (DS-v4-pro) | QSemOS (DS-v4-pro) | QSemOS (GLM-5.1) |
| ----- | :----------: | :------------: | :-----------: |
| 任务数   |     538     |      237      |     237      |
| 通过率   |    90.7%    |     83.1%     |    81.4%     |
| 编译失败  |  14 (2.6%)  |   10 (4.2%)   |  13 (5.5%)   |
| 运行崩溃  |  11 (2.0%)  |   8 (3.4%)    |   0 (0.0%)   |
| 测试失败  |  19 (3.5%)  |  22 (9.3%)   |  31 (13.1%)  |
| 完整性违规 |  6 (1.1%)   |   1 (0.4%)    |   5 (2.1%)   |

***

## 2. 失败大类分布

```
RIOT (50 failed):
  测试失败     38.0%
  编译失败     28.0%
  运行崩溃     22.0%
  完整性违规    12.0%

QSemOS DS-v4-pro (39 failed):
  测试失败     53.8%
  编译失败     25.6%
  运行崩溃     20.5%

QSemOS GLM-5.1 (43 failed):
  测试失败     69.8%
  编译失败     30.2%
  运行崩溃      0.0%
```

### 2.1 GLM-5.1 失败细分

| 大类 | 子类 | 数量 | 任务示例 |
|------|------|:---:|------|
| 模型幻觉 | 对称命名推理 | 2 | 77, 209 |
| | 功能推断编造 | 4 | 37, 79, 108, 210 |
| | 符号环境错配 | 6 | 179, 210, 226, 230, 206 |
| | 命名猜测 | 3 | 152, 162, 174 |
| 模型理解能力 | 调用步骤遗漏 | 5 | 151, 153, 221 |
| | 关联状态漏同步 | 8 | 80, 82, 84, 89 |
| | 编码约定误读 | 5 | 72, 132, 42, 45 |
| | 语义反转 | 5 | 151, 153, 88, 114, 92 |
| | 路径混淆 | 4 | 141, 191, 185 |
| | 边界条件偏差 | 5 | 142, 218, 224, 94 |
| 上下文不可获取 | 字符串字面量 | 3 | 1, 5, 133 |

***

## 3. 核心差异：根因三级分类

### 3.1 编译失败

RIOT 14 例 compile\_error 按根因：

| 根因                                             |  数量 |   占比  |
| ---------------------------------------------- | :-: | :---: |
| 幻觉 API（如 `cib_get_unsafe`、`clist_find_before`） |  6  | 42.9% |
| 编译警告（`-Werror=unused-function` 等）              |  4  | 28.6% |
| 类型错误 / 缺少头文件 / 其他                              |  4  | 28.6% |

QSemOS 10 例 compile\_error：全部是幻觉（`memcpy_s`、`OsHwiFuncGet` 等），占 100%。

**发现**: 幻觉并非闭源独有。RIOT 的 42.9% 幻觉率说明开源环境中模型也会编造 API（对称命名推理：看到 `cib_get` → 假设存在 `cib_get_unsafe`）。`-Werror` 类（28.6%）需细看：4 例中仅 task 92、136 去掉 `-Werror` 即可通过——警告来自未使用的预存基础设施函数，模型生成代码逻辑正确；task 53、112 去掉 `-Werror` 后暴露出深层的逻辑错误（返回值偏差、路由发现路径缺失），会转为测试失败。

### 3.2 运行崩溃

**RIOT (11 例)** — 网络协议栈指针操作 + NULL/边界检查缺失：

| 类型          |  数量 | 示例                                                                                                   |
| ----------- | :-: | ---------------------------------------------------------------------------------------------------- |
| 网络协议栈指针操作   |  4  | `gnrc_netreg_getnext`、`gnrc_pktbuf_mark`、`vfs_stat`、`gnrc_pkt_delete`                                |
| NULL/边界检查缺失 |  7  | `ipv6_addr_match_prefix` (未检查NULL)、`fmt_hex_bytes` (奇数长度越界)、`gnrc_pktqueue_remove` (未检查queue/node) 等 |

> 后 7 例原被 RIOT harness 误标为 `test_not_executed`。原因是 crash regex `(?:Segmentation\s+fault|Aborted|\bpanic\b)` 不匹配 AddressSanitizer 的 `SEGV`/`ABORTING` 格式。

> **QSemOS harness bug**: 同样的问题在 QSemOS 更严重——同一 crash regex 将 17 例实际通过的测试误判为 crash（ipc/signal 10 + kernel/task 7），测试输出含 "signal" 被正则误匹配。whitebox 重检 `Pass-Rate: 100%` + `0 FAILs` 后已修正为 PASS。两套 benchmark 都应改用 exit code 判定。

**QSemOS (6 例)** — 内核同步原语缺失：`OsFirstTimeSwitch`（缺 `TSK_STATUS_SET`）、`OsSwTmrGetRemainTick`（缺 `OsIntLock`）等。

**结论**: RIOT 崩溃以 NULL/边界检查缺失为主（7/11），QSemOS 以内核同步缺失为主。两者都是"条件判断偏差"的极端形式。

### 3.3 测试失败

对 19 个 RIOT 测试失败做 diff-vs-oracle 三级分类（详见 `whitebox_riot.py`）：

| 根因                           |      RIOT      |     QSemOS     |
| ---------------------------- | :------------: | :------------: |
| 条件判断偏差 (missing/extra guard) |    3 (15.8%)   | **17 (70.8%)** |
| 算法/逻辑错误                      | **11 (57.9%)** |    5 (20.8%)   |
| 架构/API 误用                    |    5 (26.3%)   |    2 (8.3%)    |

**Fisher Exact p = 0.00054**，差异极显著。

RIOT 19 例分散在 17 种子类（各 1-2 例）：运算符错误、循环逻辑错误、累加器精度、数据结构顺序、语义误解 等，反映"通用编程能力"的随机失误。

QSemOS 24 例高度集中在 conditional guard（70.8%）：`if (x != NULL)` 漏掉或 `if (usedSize > 0)` 多加了。反映"信息稀缺下的系统性策略偏差"——缺少 API 文档时模型倾向于防御过度或防御不足。

### 3.4 完整性控制

| <br /> | RIOT               | QSemOS                       |
| ------ | ------------------ | ---------------------------- |
| 隔离方式   | Docker + git reset | chmod 444 + tree-sitter mask |
| 违规率    | 1.1% (6/538)       | 0.4% (1/237)                 |

***

## 4. 重合总结

| 失败类型           |      RIOT      |     QSemOS     |    重合？    |
| -------------- | :------------: | :------------: | :-------: |
| 幻觉 API 编译      |   主要 (42.9%)   |    主要 (100%)   |  **高度重合** |
| -Werror 编译     |   次要 (28.6%)   |        无       |  RIOT 独有  |
| NULL/边界缺失崩溃    |   主要 (63.6%)   |        无       |  RIOT 独有  |
| 内核同步缺失崩溃       |        无       |    主要 (4/7)    | QSemOS 独有 |
| 复杂数据结构崩溃       |       4 例      |       3 例      |   都有的模式   |
| 条件判断偏差 (test)  |   次要 (15.8%)   | **主要 (70.8%)** |     --    |
| 算法/逻辑错误 (test) | **主要 (57.9%)** |   次要 (20.8%)   |     --    |
| 完整性违规          |       6 例      |       1 例      |    5 例    |  RIOT 更严重 |

***
## 5. QSemOS 多模型对比

### 5.1 核心差异

| 差异点 | DS-v4-pro | GLM-5.1 |
|--------|:---:|:---:|
| 通过率 | **83.5%** (198/237) | **81.9%** (194/237) |
| 运行崩溃 | **8 例**（内核同步/指针） | **0 例** |
| 测试失败 | **21 例**（53.8%） | **30 例**（69.8%） |
| 编译失败 | **10 例**（25.6%） | **13 例**（30.2%） |
| 完整性违规 | **1 例**（0.4%） | **5 例**（2.1%） |
| 中位耗时 | 108s | 131s |

GLM-5.1 零崩溃，但测试失败多 7 例。

### 5.2 模块通过率对比

| 模块 | DS-v4-pro | GLM-5.1 |
|------|:---:|:---:|
| utility/lib | 100% (8/8) | 100% (8/8) |
| core/extend | 91% (10/11) | 82% (9/11) |
| utility/log | 88% (14/16) | 81% (13/16) |
| core/ipc | 85% (53/62) | 76% (47/62) |
| extended/perf | 75% (30/40) | 82% (33/40) |
| core/kernel | 82% (84/103) | 82% (84/103) |

最悬殊：`core/ipc`（GLM 低 9 个百分点）、`extended/perf`（GLM 高 7 个百分点）。IPC 信号量/队列的复杂同步逻辑是 GLM 的短腿。

### 5.3 交汇矩阵：DS-v4-pro × GLM-5.1

```
237 tasks:
  两者都通过    172 (72.3%)
  两者都失败     18 ( 7.6%)
  GLM通过/DS失败  22 ( 9.2%)  ← DS独有的失败
  DS通过/GLM失败  26 (10.9%)  ← GLM独有的失败
```

**DS 独有失败 22 例**（GLM 能过的）：6 例崩溃（`OsHwiHookDispatcher`, `OsFirstTimeSwitch`, `PRT_LocaleCurrent`, `OsSwTmrGetRemainTick` 等）+ 条件判断偏差的 test_failure 13 例 + 幻觉编译失败 3 例。

**GLM 独有失败 26 例**（DS 能过的）：16 例 test_failure（`OsHwiDefaultHandler`, `OsTskEntry`, `PRT_TaskLock` 等）+ 10 例 compile_error（`OsCurCycleGet64`, `OsSigDefaultHandler`, `OsQueueCreate` 等全部为幻觉 API）。

**两者都失败 18 例**：内核信号/调度核心函数（`OsSignalWaitSche`, `PRT_SignalMask`, `OsFirstTimeSwitch` 等），两个模型都搞不定。

### 5.4 前 24 个任务两模型对照

| Task | 函数 | DS-v4-pro | GLM-5.1 |
|------|------|:---:|:---:|
| 1 | OsPerfDefaultNotify | PASS | **FAIL** |
| 4 | OsPerfOutPutFlush | **FAIL** | PASS |
| 6 | OsPerfOutPutInit | **FAIL** | PASS |
| 7 | OsPerfOutPutRead | **FAIL** | PASS |
| 8 | OsPerfOutPutRemainSize | PASS | PASS |
| 12 | OsTaskPrioritySetCheck | **FAIL** | PASS |
| 19 | OsSwTimerGroupCreate | PASS | **FAIL** |
| 22 | OsSwTmrCreateTimerParaChk | **FAIL** | PASS |
| 其余 16 个 | — | PASS | PASS |

| | 通过率 | 总耗时 | 中位耗时 |
|---|:---:|:---:|:---:|
| DS-v4-pro | 19/24 (79.2%) | 78min | 113s |
| GLM-5.1 | 22/24 (91.7%) | 82min | 145s |

DS-v4-pro 在前 24 个上差的 5 个全部是条件判断偏差。GLM 差 2 个——一个 printf 字符串偏差（task 1）、一个定时器组初始化偏差（task 19）。

### 5.5 策略风格总结

**DS-v4-pro**：能力更强但过度推理——编造 API、硬写 spinlock 空壳、缺同步原语。通过率高是"赌对了"，崩溃是"赌错了"。

**GLM-5.1**：保守不冒险。零崩溃说明避开了内核同步等高风险区域，代价是更多任务精度不够。

### 5.6 失败模式对比：DS-v4-pro × GLM-5.1

| 失败类型 | DS-v4-pro | GLM-5.1 | 差异解读 |
|----------|:---:|:---:|------|
| 幻觉 API（内部函数） | 6 | **8** | GLM 编造更分散，DS 集中在 C11 Annex K |
| 幻觉常量/宏 | 2 | **3** | GLM 多出 `OS_NO_WAIT`、`OS_QUEUE_BUTT`、`LOAD_FENCE` |
| 宿主环境假设 | 2 | 2 | 持平 |
| 条件判断偏差 (missing + extra) | **17** (70.8%) | 14 (45.2%) | DS 高度集中于此，是系统性策略偏差 |
| 语义漂移 | 2 | **7** | GLM spinlock 三兄弟全线漂移 |
| 计算/逻辑错误 | 4 | 5 | 持平 |
| 错误处理偏差 | 2 | 3 | 持平 |
| 字符串/常量偏差 | 0 | **2** | GLM 独有——闭源代码字符串无法推断 |
| 数据结构误用 | 2 | 1 | 持平 |
| 运行崩溃 | **7** | **0** | 根本性差异 |

**关键发现**：

1. **条件判断偏差是 DS 的"大缺点"**：70.8% 的 test_failure 是缺/多 if 条件，说明 DS 在信息稀缺下采取"防御性推断"策略——但推断方向不稳定，时而防御不足（缺少 NULL 检查），时而防御过度（添加不必要的条件）。

2. **GLM 用语义漂移换零崩溃**：DS 的 6 例崩溃源于 spinlock 空壳、缺同步原语、缺 NULL 检查。GLM 在同样场景下选择保守实现（spinlock 退化为无操作而非空壳），避免了崩溃但导致测试失败。这是"硬写但不完整"vs"保守不冒险"的策略差异。

3. **GLM 引入新失败类型**：语义漂移（7 vs 2）和字符串/常量偏差（2 vs 0）是 GLM 独有的弱点。spinlock 三兄弟 (151/152/153) 两模型都失败但方式不同——DS 崩溃、GLM 测试失败——说明内核同步是两者的共同盲区。

4. **两者的幻觉模式不同**：DS 的幻觉高度集中于 C11 安全函数迁移 (`memcpy_s` ×4) 和对称命名推理 (`OsHwiFuncGet` ← `OsHwiFuncSet`)，GLM 的编造更分散——从 backtrace 子系统到共享内存常量到 ELF 类型。

***

## 6. 失败根因分类

QSemOS 上 DS 和 GLM 合计 90 个失败归入三个大类：幻觉、理解偏差、指令遵循失败（已剔除 2 个上下文不可获取任务）。

### 6.1 幻觉（23 例，全部编译失败）

- **对称命名推理**（4 例）：有 `OsHwiFuncSet` → 编造 `OsHwiFuncGet`（104 DS）、有 `OsHandleUnBlockSignal` → 编造 `OsHandleOneSignal`（77 DS+GLM）
- **功能推断编造**（7 例）：为实现功能凭空编了一组不存在的辅助函数，如 `PerfBackTrace`+`OsPerfBackTrace`（108 GLM）、`OsSemPendParaCheck`+`OsSemPendNotNeedSche`+`OsSemPendListPut`（165 DS）
- **符号环境错配**（8 例）：符号在 Linux/POSIX 下正确但嵌入式编译链不提供——`memcpy_s`/`memset_s`（C11 Annex K）、`ELFMAG`/`Elf_Rela`（POSIX elf.h）、`U64`/`U16`（Linux typedef）
- **命名猜测**（4 例）：知道要某个语义但猜错名字——`OS_NO_WAIT`(162 GLM)、`LOAD_FENCE`/`STORE_FENCE`(206 GLM)

GLM（15 例）显著多于 DS（8 例），尤其是符号环境错配（6 vs 2）。

### 6.2 理解偏差（QSemOS 61 / RIOT ~34，测试失败 + 崩溃）

- **条件判断偏差**（QSemOS 11 / RIOT 10）：QSemOS 多了/少了/错了条件守卫——`usedSize>0` 堵 flush、漏 `OS_TSK_LOCK` 检查导致死锁。RIOT 7 例 NULL/边界检查缺失导致崩溃 + 3 例条件判断偏差导致测试失败。
- **步骤不完整**（QSemOS 23 / RIOT 4）：QSemOS spinlock irqsave 四步协议缺步、signal 三字段联动漏字段。RIOT 网络协议栈指针操作（`gnrc_netreg_getnext`、`gnrc_pktbuf_mark`）需要多步操作，模型只做了其中一步。
- **逻辑错误**（QSemOS 10 / RIOT 11）：QSemOS 白名单 vs 黑名单反转（task 114）、wraparound 方向算反（task 92）。RIOT 11 例算法/逻辑错误分散在 17 种子类，是通用编程能力的随机失误。
- **表示误读**（QSemOS 8 / RIOT 4）：QSemOS 链表偏移量当绝对时间戳（task 66）、位掩码当普通枚举（task 72）。RIOT 部分算法错误源于对内部数据结构格式的误读。
- **上下文错配**（QSemOS 8 / RIOT 5）：QSemOS 冷启动用热切换逻辑（task 141）、绕过 `OsHwiHandleActive` 派发（task 104）。RIOT 选错中断注册路径、调用不适合上下文的 API。

### 6.3 两套 benchmark 的分布差异

| 子类 | QSemOS 侧重 | RIOT 侧重 |
|------|-----------|---------|
| 条件判断偏差 | 条件守卫（多了/少了/顺序错了） | NULL/边界检查缺失（63.6% 崩溃） |
| 步骤不完整 | 内核同步协议（spinlock、signal 状态机） | 网络协议栈多步操作 |
| 逻辑错误 | 语义反转、方向错误 | 算法/逻辑离散失误（17 种子类） |
| 表示误读 | 闭源定制的位掩码、偏移量编码 | 内部数据结构格式误读 |
| 上下文错配 | 冷热路径混淆、绕过标准派发 | 中断注册/API 上下文选错 |

两套 benchmark 考验的维度互补：QSemOS 侧重闭源环境下的内核同步协议和编码推断，RIOT 侧重开源环境下的参数校验和通用编程能力。符号环境错配在 RIOT 为零（开源头文件全部可见），是闭源 benchmark 独有的失败模式。文件系统和内存管理在 QSemOS 的 237 个 task 中未涉及，模型在此的表现未知。

### 6.4 OS 模块架构

QSemOS 和 RIOT 的功能模块覆盖存在系统性差异：

| 功能域 | QSemOS 模块 | QSemOS 通过率 | RIOT 模块 | RIOT 通过率 |
|--------|------------|:---:|------|:---:|
| **任务调度** | kernel/task, sched, task_init | ~80% | core/thread | ~94% |
| **中断管理** | kernel/irq | 88.9% | core/irq | ~92% |
| **IPC 信号量** | ipc/sem | 85.7% | sys/sem | ~88% |
| **IPC 消息队列** | ipc/queue | 84.6% | core/msg | ~86% |
| **IPC 信号** | ipc/signal | 73.3% | — | — |
| **IPC 读写锁** | ipc/rwlock | 100% | — | — |
| **同步 自旋锁** | kernel/spinlock | 66.7% | — | — |
| **同步 互斥锁** | — | — | sys/mutex | ~91% |
| **定时器** | kernel/timer | 89.3% | sys/xtimer | ~86% |
| **动态模块加载** | extend/dlmodule | 90.9% | — | — |
| **性能监控** | extended/perf | ~75% | — | — |
| **日志系统** | utility/log | ~85% | sys/log | ~89% |
| **内存管理** | — | — | core/mem | ~93% |
| **文件系统** | — | — | sys/vfs | ~86% |
| **网络协议栈** | — | — | net/gnrc | ~79% |

QSemOS 独有模块集中在**内核同步**（信号、读写锁、自旋锁）和**性能可观测性**（perf、动态模块加载），这正是模型理解偏差最集中的区域——spinlock 通过率 66.7%、signal 通过率 73.3%。RIOT 独有的内存管理、文件系统、网络协议栈在 QSemOS 上无对应测试，模型在这些模块的表现未知。

### 6.5 指令遵循失败（QSemOS 6 / RIOT 6）

chmod 444 + tree-sitter mask 机制约束了编辑范围，但模型有时会修改 mask 区域外的代码或非目标文件。GLM 5 例越界违规、DS 1 例。越界编辑本身不影响代码生成质量判定，但反映了模型在指令遵循上的系统性差异——GLM 更倾向于在工具约束边界试探。

```

● 全部失败 (QSemOS 82 + RIOT ~44，越界编辑不在列)
  ├── 幻觉 (23，全部编译失败)
  │   ├── 对称命名推理 (4)
  │   ├── 功能推断编造 (7)
  │   ├── 符号环境错配 (8)
  │   └── 命名猜测 (4)
  │
  ├── 理解偏差 (QSemOS 61 + RIOT ~34，测试失败 + 崩溃)
  │   ├── 条件判断偏差 (QSemOS 11 / RIOT 10)
  │   ├── 步骤不完整 (QSemOS 23 / RIOT 4)
  │   ├── 逻辑错误 (QSemOS 10 / RIOT 11)
  │   ├── 表示误读 (QSemOS 8 / RIOT 4)
  │   └── 上下文错配 (QSemOS 8 / RIOT 5)
  │
  └── 指令遵循失败 (QSemOS 6 / RIOT 6)
      ├── 越界编辑 (6)
      └── 完整性违规 (与越界编辑同源)
```





