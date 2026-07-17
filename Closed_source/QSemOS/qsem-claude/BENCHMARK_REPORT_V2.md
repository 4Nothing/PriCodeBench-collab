# QSem-Claude Benchmark 全量报告

**数据集**: PrivateAPIEval\_agent.jsonl (240 tasks)
**数据重建**: 176 个缺失源文件从 PrivateAPIEval.jsonl 的 task\_input/ref\_code/suffix\_code 重建
**调用方式**: Claude Code CLI（通过硅基流动 / CC-Switch 路由）

## 1. 多模型总览

| 指标 | DS-v4-pro | GLM-5.1 | kimi-k2.7-code* |
|------|:---:|:---:|:---:|
| 通过率 | **82.9%** (199/240) | **80.8%** (194/240) | **95.8%** (23/24) |
| 编译失败 | 10 (4.2%) | 13 (5.4%) | 1 (4.2%) |
| 运行崩溃 | 7 (2.9%) | 0 (0.0%) | 0 (0.0%) |
| 测试失败 | 24 (10.0%) | 33 (13.8%) | 0 (0.0%) |
| 完整性违规 | 1 (0.4%) | 5 (2.1%) | 1 (4.2%) |
| 中位耗时 | 108s | 131s | 237s |
| 总耗时 | ~15h | 11.6h | 1.9h (仅24任务) |

> *kimi-k2.7-code 仅完成前 24/240 个任务，数据仅供参考。

***

## 2. DS-v4-pro 详细分析

### 2.1 模块通过率

| 模块                     | 任务数 |  通过 |   通过率  |
| ---------------------- | :-: | :-: | :----: |
| `ipc/rwlock`           |  13 |  13 | 100.0% |
| `utility/prt_notifier` |  8  |  8  | 100.0% |
| `perf/prt_perf`        |  15 |  14 |  93.3% |
| `extend/dlmodule`      |  11 |  10 |  90.9% |
| `kernel/timer`         |  28 |  25 |  89.3% |
| `kernel/irq`           |  9  |  8  |  88.9% |
| `utility/log`          |  16 |  14 |  87.5% |
| `ipc/sem`              |  21 |  18 |  85.7% |
| `ipc/queue`            |  13 |  11 |  84.6% |
| `kernel/sys`           |  6  |  5  |  83.3% |
| `kernel/task_init`     |  16 |  13 |  81.2% |
| `kernel/sched`         |  5  |  4  |  80.0% |
| `kernel/task`          |  10 |  8  |  80.0% |
| `kernel/task_lock`     |  10 |  8  |  80.0% |
| `kernel/task_priority` |  5  |  4  |  80.0% |
| `perf/ringbuf`         |  9  |  7  |  77.8% |
| `ipc/signal`           |  15 |  11 |  73.3% |
| `perf/output`          |  9  |  6  |  66.7% |
| `kernel/spinlock`      |  9  |  6  |  66.7% |
| `kernel/task_info`     |  5  |  3  |  60.0% |
| `perf/pmu`             |  7  |  3  |  42.9% |

***

### 2.2 失败分类

41 个失败按根因分为 3 大类 13 子类：

### 失败大类

```
test_failure      24 (58.5%)
compile_error     10 (24.4%)
crash              7 (17.1%)
```

### 失败细分

| 失败类型 | 具体原因 | 数量 | 说明 |
| -------------- | ------------------------------------------- | :-: | -------------------- |
| test\_failure  | **missing\_conditional\_guard**             |  9  | 缺少 oracle 中的保护性条件    |
| test\_failure  | **extra\_conditional\_guard**               |  8  | 添加了 oracle 中不存在的限制条件 |
| compile\_error | **undefined\_reference**                    |  7  | 引用了不存在的符号，链接失败       |
| test\_failure  | **wrong\_calculation**                      |  4  | 位运算/算术逻辑与 oracle 不一致 |
| compile\_error | **hallucinated\_api**                       |  3  | 编造了不存在的 API          |
| test\_failure  | **wrong\_error\_handling**                  |  2  | 错误处理返回值与 oracle 不一致  |
| test\_failure  | **semantic\_drift**                         |  2  | 结构相似但行为有偏差           |
| crash          | **data\_structure\_misuse**                 |  2  | 链表/状态机操作错误导致崩溃       |
| crash          | **missing\_synchronization**                |  2  | 缺少锁/关中断保护导致崩溃        |
| crash          | **hallucinated\_api + wrong\_architecture** |  1  | 编造 API 并绕过了内核标准派发路径  |
| crash          | **missing\_null\_check**                    |  1  | 缺少空指针检查导致崩溃          |

***

### 2.3 失败模式详解

#### 条件判断偏差 — 24 个 test_failure 的核心问题

70.8% 的 test\_failure (17/24) 是条件判断偏差，分为两类：

**Missing conditional guard (9 tasks)** — Claude 缺少 oracle 中的保护性检查：

| Task | 函数                       | 缺少的条件                                                          |
| ---- | ------------------------ | -------------------------------------------------------------- |
| 6    | `OsPerfOutPutInit`       | `buf == NULL` 时分配内存的分支                                         |
| 12   | `OsTaskPrioritySetCheck` | `taskPrio > OS_TSK_PRIORITY_LOWEST` / `CHECK_TSK_PID_OVERFLOW` |
| 35   | `PRT_SemCreate`          | `count > OS_SEM_COUNT_MAX` 检查                                  |
| 38   | `OsSetSysTimeHook`       | `g_sysTimeHook != NULL` 检查                                     |
| 66   | `OsSwTmrGetRemain`       | 定时器链表遍历 (`while` loop)                                         |
| 92   | `OsRingbufWriteLinear`   | `(ringbuf->remain < size) ? ringbuf->remain : size` 计算         |
| 170  | `OsGetSrcPid`            | `OS_HWI_ACTIVE` 检查 + SMP 条件编译分支                                |
| 175  | `PRT_QueueRead`          | `innerId >= g_maxQueue` + NULL 参数检查                            |
| 240  | `OsTaskSpPcInfoReady`    | `OsTaskStackIsSave` + 运行状态检查                                   |

**Extra conditional guard (8 tasks)** — Claude 添加了 oracle 不存在的限制条件：

| Task | 函数                          | Claude 额外添加的条件                      | 后果                  |
| ---- | --------------------------- | ----------------------------------- | ------------------- |
| 4    | `OsPerfOutPutFlush`         | `usedSize > 0` 检查后才调用 flush hook    | 空 buffer 时 hook 不触发 |
| 7    | `OsPerfOutPutRead`          | `usedSize < size` 后返回 OS\_ERROR     | 改变了函数语义             |
| 22   | `OsSwTmrCreateTimerParaChk` | `createPara == NULL` 检查             | 参数校验逻辑不同            |
| 123  | `OsIdleTaskExe`             | 条件判断重构                              | idle 任务执行逻辑偏差       |
| 135  | `OsPerfTimedConfig`         | 合并两个独立检查为复合条件                       | event 配置逻辑错误        |
| 162  | `OsSemPendParaCheck`        | 重构 `OS_TASK_LOCK_DATA` 检查           | 信号量参数检查偏差           |
| 208  | `PRT_Log`                   | 丢失 `strLen > (LOG_MAX_SIZE - 1)` 检查 | log 长度校验不完整         |
| 218  | `PRT_TaskDelay`             | `UNI_FLAG == 0` 条件包围                | 调度逻辑受限制             |

#### 幻觉 API — 10 个编译失败

Claude 编造了以下不存在的符号：

| 幻觉符号                  |  次数 | Task | 推测原因                           |
| --------------------- | :-: | ---- | ------------------------------ |
| `memcpy_s`            |  4  | 236  | C11 Annex K 安全函数，OS kernel 未实现 |
| `OsDynModuleSetError` |  2  | 227  | 推理出 "有 Find 就有 SetError"       |
| `OsHwiFuncGet`        |  1  | 104  | 真实存在 `OsHwiFuncSet`，对称命名推理     |
| `OsHwiParaGet`        |  1  | 104  | 真实存在 `OsHwiParaSet`，对称命名推理     |
| `OsHandleOneSignal`   |  1  | 77   | 信号模块内部函数，Claude 实现中未提供         |
| `PRT_SplLockInit`     |  1  | 94   | 应使用 `OsSpinLockInitInner`      |
| `PRT_TimerStop`       |  1  | 138  | 不存在此 API                       |
| `OsIntLock`           |  1  | 165  | 拼写偏差                           |
| `OsSpinLockInitInner` |  1  | 193  | 跨模块调用，当前上下文不可见                 |
| `PRT_KERNEL_CORE_NUM` |  1  | 120  | 编造的宏                           |

**幻觉模式规律**:

- **C11 安全函数迁移**: Claude 习惯用 `memcpy_s` 替代 `memcpy`，但嵌入式 kernel 通常不实现 Annex K
- **对称命名推理**: 看到 `OsHwiFuncSet`/`OsHwiParaSet` 就假设存在对应的 `Get` → 经典 LLM 幻觉模式
- **跨模块可见性不足**: 部分 API 在 QSemOS 其他模块存在但当前测试的 include 路径中不可见

#### 内核复杂逻辑 — 7 个崩溃/异常

**Spinlock 硬件操作 (3 tasks)**: tasks 151 (`PRT_SplLock`)、153 (`PRT_SplUnlock`) 实现为 `(void)spinLock;` 空壳。这些函数需要直接操作硬件 spinlock 寄存器 (`OsSplLock(addr)`)，Claude 缺少足够上下文推断。Task 152 (`PRT_SplLockInit`) 同样缺少 `OsSplLockInit` 初始化调用。

**中断分发架构错误 (1 task)**: Task 104 (`OsHwiHookDispatcher`) — Claude 编造了 `OsHwiFuncGet`/`OsHwiParaGet` 来手动获取和调用中断处理函数，绕过了内核标准的 `OsHwiHandleActive` 派发路径。这说明 Claude 理解"中断最终要调用处理函数"，但不理解 QSemOS 的 IRQ 框架封装。

**调度器数据结构误操作 (2 tasks)**: Task 82 (`OsSignalTimeOutSet`) 缺少 `TSK_STATUS_CLEAR` 操作。Task 141 (`OsFirstTimeSwitch`) 缺少 `TSK_STATUS_SET` 标记调度状态，这是内核启动时第一次上下文切换的关键操作。

**同步原语缺失 (2 tasks)**: Task 67 (`OsSwTmrGetRemainTick`) 缺少 `OsIntLock`/`OsIntRestore` 保护。Task 152 缺少 spinlock 初始化。

**空指针检查缺失 (1 task)**: Task 197 (`PRT_LocaleCurrent`) 缺少 2 个 NULL 检查，oracle 有而 Claude 没有。

***

### 2.4 错误处理偏差

#### Task 80: OsSigWaitProcPendingSignal

Claude 在无匹配信号时返回 `OS_ERRNO_SIGNAL_PARA_INVALID`，而 oracle 的设计语义是"轮询"而非"出错"——无信号匹配时返回 `OS_OK` 并清零 `sigWaitMask`。

此外，Claude 在匹配到信号后过早 return，跳过了 `sigWaitMask = 0` 清理操作，导致 wait mask 状态不一致。

#### Task 88: PRT_SignalMask

Claude 在 switch 的 `default` 分支（无效的 `how` 值）返回错误码并提前解锁退出。Oracle 不做任何处理，径直 fall-through 到统一返回点。导致 1 个测试用例（"pending cleared after unblock"）失败。

***

### 2.5 Token 与成本分析

| 指标      | 数值         |
| ------- | ---------- |
| 总 Token | 15,039,090 |
| 缓存命中率   | 92.0%      |
| 实际花费    | ¥53        |
| 平均每任务   | ¥0.22      |

***

### 2.6 耗时分析

| 分位数 | 耗时    |
| --- | ----- |
| P10 | 44s   |
| P25 | 64s   |
| P50 | 108s  |
| P75 | 188s  |
| P90 | 364s  |
| P95 | 536s  |
| Max | 2108s |

### 最慢 10 任务

| Task |   耗时  | 函数                     | 模块             |       结果       |
| ---- | :---: | ---------------------- | -------------- | :------------: |
| 131  | 2108s | OsTskReadyDel          | task           |      PASS      |
| 191  | 1478s | OsTskEntry             | task\_init     |      PASS      |
| 12   | 1444s | OsTaskPrioritySetCheck | task\_priority |      fail      |
| 183  | 1334s | OsActivate             | task\_init     | compile\_error |
| 157  |  775s | OsPriorityRestore      | sem            |      PASS      |
| 197  |  676s | PRT\_LocaleCurrent     | task\_init     |      crash     |
| 140  |  641s | OsContextSwitch        | sched          |      PASS      |
| 141  |  601s | OsFirstTimeSwitch      | sched          |      crash     |
| 87   |  584s | PRT\_SignalDeliver     | signal         |      PASS      |
| 144  |  581s | OsMainSchedule         | sched          |      PASS      |

10 个最慢任务全部集中在 task/sched/signal 三个内核模块。4 个最终 PASS，说明更长的推理时间确实带来了正收益。

***

### 2.7 结论

1. **通过率 82.9%** (199/240): DeepSeek-v4-pro 在嵌入式 OS 内核函数补全任务上的正确率约为 5/6
2. **完整性 99.6%** (239/240): chmod 权限锁定 + tree-sitter mask 机制有效约束了编辑范围
3. **条件判断偏差是最主要的失败模式** — 70.8% 的 test\_failure 是缺少或多余的条件保护
4. **幻觉 API 呈现两种规律**: C11 安全函数迁移（`memcpy_s`）和对称命名推理（`OsHwiFuncGet` ← `OsHwiFuncSet`）
5. **硬件操作是盲区**: spinlock/prt\_hw 等需要平台特定知识的模块，Claude 无法单凭代码上下文推断
6. **最薄弱模块**: pmu (42.9%)、task\_info (60.0%)、spinlock (66.7%)、perf/output (66.7%)
7. **缓存命中 92.0%**: 源文件读取在多次 API 调用间被有效缓存，总成本仅 ¥53/240 任务
8. **内核调度/任务管理模块耗时最长**: 10 个最慢任务全部来自 task/sched/signal，建议针对此类函数增加结构化推理引导

***

## 3. GLM-5.1 详细分析

### 3.1 总览

| 指标 | 数值 |
|------|:---:|
| 通过率 | 80.8% (194/240) |
| 编译失败 | 13 (5.4%) |
| 测试失败 | 33 (13.8%) |
| 运行崩溃 | 0 (0.0%) |
| 完整性违规 | 5 (2.1%) |
| 总耗时 | 11.6h |

零崩溃——DS-v4-pro 7 例崩溃涉及的内核同步/指针操作，GLM-5.1 全部避开。代价是测试失败多 9 例、编译失败多 3 例。

### 3.2 模块通过率

| 模块 | DS-v4-pro | GLM-5.1 | 差距 |
|------|:---:|:---:|:---:|
| utility/lib | 100% | 100% | 0 |
| core/kernel | 82% | 82% | 0 |
| utility/log | 88% | 81% | -7 |
| core/extend | 91% | 82% | -9 |
| core/ipc | 85% | 76% | **-9** |
| extended/perf | 75% | 82% | **+7** |

IPC 信号量/队列是 GLM 最大短板，perf 反超 DS-v4-pro。

### 3.3 与 DS-v4-pro 交汇

```
两者都通过: 172 (71.7%)
两者都失败:  19 ( 7.9%)
GLM独赢:    22 ( 9.2%)  ← DS崩溃+条件判断偏差
DS独赢:     27 (11.3%)  ← GLM幻觉编译+逻辑偏差
```

**两者都失败 19 例**：内核信号/调度核心函数（`OsSignalWaitSche`、`PRT_SignalMask`、`OsFirstTimeSwitch`），两个模型都无能为力。

### 3.4 失败分类

46 个失败按根因分为 3 大类 8 子类：

**失败大类**

```
测试失败      33 (71.7%)
编译失败      13 (28.3%)
运行崩溃       0 ( 0.0%)
```

**失败细分**

| 失败类型 | 具体原因 | 数量 | 说明 |
|----|---------|:---:|------|
| compile_error | **hallucinated_api** | 8 | 编造了不存在的内部函数 |
| compile_error | **hallucinated_constant** | 3 | 编造了不存在的枚举值/宏 |
| compile_error | **host_env_assumption** | 2 | 使用了嵌入式环境不存在的 POSIX 类型/常量 |
| test_failure | **missing_conditional_guard** | 12 | 缺少 oracle 中的保护性条件 |
| test_failure | **semantic_drift** | 7 | 结构相似但行为有偏差 |
| test_failure | **wrong_calculation** | 5 | 位运算/算术/赋值逻辑与 oracle 不一致 |
| test_failure | **wrong_error_handling** | 3 | 错误处理返回值/错误码与 oracle 不一致 |
| test_failure | **extra_conditional_guard** | 2 | 添加了 oracle 中不存在的限制条件 |
| test_failure | **hallucinated_value** | 2 | 字符串字面量/常量值与 oracle 不一致 |
| test_failure | **data_structure_misuse** | 1 | 链表/状态机操作错误 |

### 3.5 失败模式详解

#### 3.5.1 幻觉 API — 13 例编译失败

GLM-5.1 的编译失败 100% 由幻觉引起，分为三类：

**编造内部函数 (8 例)** — 对称命名推理是最主要的幻觉来源：

| 任务 | 函数 | 幻觉符号 | 推测原因 |
|------|------|----------|----------|
| 37 | `OsCurCycleGet64` | `PRT_ClkGetCycleCount64` | 看到 PRT 前缀 API，推断存在对称的 ClkGet 包装 |
| 77 | `OsHandleUnBlockSignal` | `OsHandleOneSignal` | 看到 `OsHandleUnBlockSignal`，推断存在单信号版本 |
| 79 | `OsSigDefaultHandler` | `PRT_TaskDelete` | 混淆 POSIX `SIG_DFL` 语义，编造 task delete API |
| 108 | `OsPerfCollectData` | `OsPerfBackTrace`、`PerfBackTrace`、`PRT_ClkGetCycleCount64` | 批量编造了整个 backtrace 子系统 |
| 152 | `PRT_SplLockInit` | `OsSpinLockInitInner` | 编译器建议 `OsSplLockInit`，模型猜测了 spinlock 变体 |
| 179 | `OsQueueCreate` | `OS_LIST_INIT`、`memset_s` | Linux 风格初始化宏 + C11 Annex K 安全函数 |
| 209 | `PRT_LogFormat` | `PRT_Log` | 推断存在内部 log 写入核心函数 |
| 210 | `PRT_LogInit` | `SHM_USED_SIZE`、`g_sequenceNum`、`memset_s` | 编造了日志子系统的私有状态变量 |

**编造常量/宏 (3 例)**：

| 任务 | 函数 | 幻觉符号 | 推测原因 |
|------|------|----------|----------|
| 162 | `OsSemPendParaCheck` | `OS_NO_WAIT` | 猜测"不阻塞"超时值，实际代码库使用不同命名 |
| 174 | `OsQueueWriteParaCheck` | `OS_QUEUE_BUTT` | 猜测 `_BUTT` 后缀的枚举上界（借鉴 Linux `_MAX` 模式） |
| 206 | `OsLogUpdateTail` | `LOAD_FENCE()`、`STORE_FENCE()` | 编造通用内存屏障宏，实际使用 `M_FENCE` |

**宿主环境假设 (2 例)**：

| 任务 | 函数 | 使用的符号 | 原因 |
|------|------|-----------|------|
| 226 | `OsDynModuleCheckElf` | `ELFMAG`、`SELFMAG` | POSIX `<elf.h>` 常量，嵌入式构建环境不提供 |
| 230 | `OsDynModuleProcessRelocationSection` | `Elf_Rela`、`Elf_Rel` | POSIX `<elf.h>` 类型，项目使用自定义 ELF 类型定义 |

**与 DS-v4-pro 对比**：GLM 的幻觉总量 (13) 与 DS (10) 接近但分布不同。DS 集中在 C11 安全函数迁移 (`memcpy_s` ×4) 和对称命名推理 (`OsHwiFuncGet`/`OsHwiParaGet`)，GLM 的编造更分散——从内部辅助函数到共享内存常量到 ELF 类型，覆盖面更广。两者都表现出"看到 Set 就假设存在 Get"的模式。

#### 3.5.2 条件判断偏差 — 14 例测试失败

条件判断偏差占 test_failure 的 42.4% (14/33)，远低于 DS-v4-pro 的 70.8%。

**Missing conditional guard (12 例)** — GLM 缺少 oracle 中的保护性检查：

| 任务 | 函数 | 缺少的条件 |
|------|------|-----------|
| 35 | `PRT_SemCreate` | `count > OS_SEM_COUNT_MAX` 溢出检查 |
| 49 | `OsRwLockRdPend` | 调用者已持有写锁时的自死锁防护 |
| 80 | `OsSigWaitProcPendingSignal` | `matchSet == 0` 早返回 + `sigWaitMask = 0` 清理 |
| 82 | `OsSignalTimeOutSet` | `else` 分支 `TSK_STATUS_CLEAR(OS_TSK_TIMEOUT)` |
| 84 | `OsSignalWaitSche` | `OS_TSK_LOCK` 状态检查、`sigWaitMask` 清理 |
| 89 | `PRT_SignalWait` | `*set == 0` 空信号集检查、`timeOutTick` 合法性验证 |
| 94 | `PRT_RingbufInit` | `status == RBUF_INITED` 重复初始化检查 |
| 115 | `OsPerfUpdateEventCount` | `event` 参数 NULL 检查、`cpuid` 边界检查 |
| 19 | `OsSwTimerGroupCreate` | `UNI_FLAG` 系统初始化状态检查 |
| 141 | `OsFirstTimeSwitch` | `g_highestTask` NULL 检查（桩环境为空指针） |
| 185 | `OsTaskCreateParaCheck` | `stackSize == 0` 自动赋值默认值、stack 地址溢出检查 |
| 191 | `OsTskEntry` | `RUNNING_TASK` NULL 检查、`taskEntry` NULL 检查 |

**Extra conditional guard (2 例)** — GLM 添加了 oracle 不存在的限制条件：

| 任务 | 函数 | 额外添加的条件 | 后果 |
|------|------|---------------|------|
| 142 | `OsHwiDispatchTail` | `TICK_NO_RESPOND_CNT > 0` 包围 tick 处理 | tick 计数器为零时跳过应有的派发 |
| 218 | `PRT_TaskDelay` | `UNI_FLAG == 0` 检查返回错误 | 未初始化状态下阻塞了后续任务 |

#### 3.5.3 语义漂移 — 7 例测试失败

结构正确但行为有偏差的模式，占 test_failure 的 21.2%：

| 任务 | 函数 | 漂移细节 |
|------|------|----------|
| 88 | `PRT_SignalMask` | 先清除 `sigMask` 再调用 `OsHandleUnBlockSignal`，顺序反了——应先递送待处理信号再清除 mask |
| 151 | `PRT_SplLock` | 单核路径仅调用 `OS_TSK_SPINLOCK()`（空操作），缺少 `OsIntLock()` 关中断 |
| 153 | `PRT_SplUnlock` | 单核路径为空操作 `(void)spinLock;`，缺少 `OsIntRestore()` 恢复中断 |
| 215 | `OsTaskIrqUnlock` | 调用 `OsTskUnlock()` 但语义与 oracle 的解锁序列不同 |
| 221 | `PRT_TaskLock` | `OsIntLock()`/`OS_TSK_LOCK()` 协议与 oracle 不匹配，缺少中断保存值传递 |
| 239 | `OsTaskSpPcGet` | `OsTaskSpPcInfoReady()` 判断条件与 oracle 不一致 |
| 114 | `OsPerfTaskFilter` | 过滤语义反转——实现了白名单而 oracle 是黑名单（排除列表） |

Spinlock  (151/152/153) 集中体现了 GLM 对内核同步机制的理解不足：单核环境下 spinlock 退化为关中断，但 GLM 退化为空操作。

#### 3.5.4 计算/逻辑错误 — 5 例测试失败

| 任务 | 函数 | 错误细节 |
|------|------|----------|
| 42 | `PRT_TickGetCount` | 返回 `g_uniTicks` 而非 `g_uniTicks + g_tickOffset`，漏掉偏移量 |
| 45 | `OsRwLockPendFindPosSub` | 优先级比较使用严格 `>`，应使用 `>=` |
| 72 | `OsSwTmrStopTimer` | 状态赋值 `state = OS_TIMER_CREATED` 覆盖所有位，应只清除状态位保留 flag |
| 92 | `OsRingbufWriteLinear` | 可用空间计算 `startIdx - endIdx`，`startIdx <= endIdx` 时错误 |
| 175 | `PRT_QueueRead` | 截断读取后 `*len = msgSize`（原始大小），应为 `*len = copySize`（实际拷贝量） |

#### 3.5.5 错误处理偏差 — 3 例测试失败

| 任务 | 函数 | 错误细节 |
|------|------|----------|
| 100 | `OsHwiDefaultHandler` | 记录了 HWI 编号但未调用 `OS_REPORT_ERROR` 上报错误 |
| 165 | `PRT_SemPend` | 超时唤醒后检测 `OS_TSK_PEND` 标志——但此时标志已被清除，超时被忽略 |
| 224 | `PRT_TaskYield` | 先检查 `OS_INT_ACTIVE` 后检查参数有效性，导致参数错误返回码被上下文错误码覆盖 |

#### 3.5.6 字符串/常量偏差 — 2 例测试失败

| 任务 | 函数 | 偏差 |
|------|------|------|
| 1 | `OsPerfDefaultNotify` | 打印 `"perf buf water mark!"`，oracle 为 `"perf buf waterline notify!"` |
| 133 | `OsPerfGetEventName` | 返回 `"cpu-clock"`（Linux perf 约定），oracle 为 `"timed"` |

这两个是"无上下文猜测"的典型——闭源代码库中字符串字面量无法从上下文推断，模型只能依赖训练数据中的常见模式（Linux perf 命名）。

### 3.6 与 DS-v4-pro 失败模式对比

| 失败类型 | DS-v4-pro | GLM-5.1 | 差异解读 |
|----------|:---:|:---:|------|
| 幻觉 API 编译 | 10 (24.4%) | 13 (28.3%) | 数量接近，GLM 更分散 |
| 条件判断偏差 (test) | **17 (41.5%)** | 14 (30.4%) | DS 更严重 |
| 条件偏差占 test_failure | **70.8%** | **42.4%** | DS 高度集中于此 |
| 语义漂移 (test) | 2 (4.9%) | **7 (15.2%)** | GLM 更严重 |
| 计算/逻辑错误 (test) | 4 (9.8%) | 5 (10.9%) | 相近 |
| 运行崩溃 | **7 (17.1%)** | **0 (0.0%)** | 根本性差异 |
| 完整性违规 | 1 | 5 | GLM 更频繁越界编辑 |

**核心差异**：
1. **崩溃 vs 保守**：DS 的 7 例崩溃源于硬写 spinlock 空壳、缺同步原语、缺 NULL 检查。GLM 零崩溃但代价是更多语义漂移（spinlock 退化为无操作而非空壳崩溃）和更多幻觉编译。
2. **条件判断**：DS 70.8% 的 test_failure 是条件判断偏差（系统性策略偏差），GLM 只有 42.4%——但 GLM 的语义漂移（21.2%）和幻觉值（6.1%）引入了 DS 没有的新失败类型。
3. **完整性**：GLM 5 例违规 vs DS 1 例——GLM 更倾向于越界编辑额外代码。

## 4. kimi-k2.7-code 预分析

### 4.1 前 24 任务三模型对照

| | DS-v4-pro | GLM-5.1 | kimi-k2.7 |
|---|:---:|:---:|:---:|
| 通过率 | 19/24 (79.2%) | 22/24 (91.7%) | 23/24 (95.8%) |
| 总耗时 | 78min | 82min | 114min |
| 中位耗时 | 113s | 145s | **237s** |

kimi 把 DS-v4-pro 的 5 个条件判断偏差全修掉了（tasks 4,6,7,12,22），GLM 的 2 个失败也修了。唯一翻车是 task 8 编造了不存在的锁 API `PRT_SplIrqLock`/`PRT_SplIrqUnlock`——思路正确但 API 幻觉。

***

## 5. 失败根因分类

两模型合计 87 个失败任务归入三个大类。越界编辑 6 例单独统计，不属于代码生成质量的失败根因。

### 5.1 模型幻觉（23 例，全部编译失败）

编造了代码库里不存在的函数名、宏或类型名。两模型 23 例编译失败全部属于此类。

**对称命名推理**：代码库里有一个方向的 API，模型假设反向也存在。

| 已有 API | 模型编造 | 任务 |
|----------|----------|:---:|
| `OsHwiFuncSet` / `OsHwiParaSet` | `OsHwiFuncGet` / `OsHwiParaGet` | 104 (DS) |
| `OsDynModuleFind` | `OsDynModuleSetError` | 227 (DS) |
| `OsHandleUnBlockSignal` | `OsHandleOneSignal` | 77 (DS+GLM) |
| 内部日志函数 | `PRT_Log` | 209 (GLM) |

**功能推断编造**：模型知道要实现什么功能，但不知道调用哪个 API，于是就地编了一组。

| 要实现的功能 | 模型编造的 API | 任务 |
|-------------|---------------|:---:|
| 读硬件时钟周期 | `PRT_ClkGetCycleCount64` | 37 (GLM) |
| 收集调用栈 | `PerfBackTrace` + `OsPerfBackTrace` + `PRT_ClkGetCycleCount64` | 108 (GLM) |
| 日志初始化 | `SHM_USED_SIZE` + `g_sequenceNum` | 210 (GLM) |
| 默认信号处理→终止任务 | `PRT_TaskDelete` | 79 (GLM) |
| 信号量阻塞参数检查 | `OsSemPendParaCheck` + `OsSemPendNotNeedSche` + `OsSemPendListPut` | 165 (DS) |
| 启动任务 | `OsTskHighestSet` | 183 (DS) |
| 停止性能定时器 | `PRT_TimerStop` + `PRT_GetCoreID` | 138 (DS) |

**符号环境错配**（8 例）：符号在 Linux/POSIX 下是正确的，但嵌入式编译链不提供。

| 符号 | 来源 | 任务 |
|------|------|:---:|
| `memcpy_s`（×4） / `memset_s`（×2） | C11 Annex K 安全函数 | 236 (DS), 179/210 (GLM) |
| `U64` / `U16` | Linux 内核 typedef | 202 (DS) |
| `OsAtomicReadU32` / `OsAtomicSetU32` | Linux 内核原子操作 | 202 (DS) |
| `ELFMAG` / `SELFMAG` | POSIX `<elf.h>` 宏 | 226 (GLM) |
| `Elf_Rela` / `Elf_Rel` | POSIX `<elf.h>` 类型 | 230 (GLM) |

**命名猜测**（4 例）：知道要某个语义的常量，但猜错了项目中的名字。

| 模型想要的语义 | 编造的名字 | 正确名字 | 任务 |
|---------------|-----------|---------|:---:|
| "不阻塞等待"超时值 | `OS_NO_WAIT` | — | 162 (GLM) |
| 队列优先级上限 | `OS_QUEUE_BUTT` | `OS_QUEUE_USED` | 174 (GLM) |
| 内存屏障 | `LOAD_FENCE` / `STORE_FENCE` | `M_FENCE` | 206 (GLM) |
| 链表初始化 | `OS_LIST_INIT` | — | 179 (GLM) |

### 5.2 模型理解能力（60 例，含测试失败和崩溃）

代码能编译通过，但模型对机制运作方式的理解有偏差。五个子类覆盖 QSemOS 和 RIOT 两套 benchmark。

**条件判断偏差**（QSemOS 11 / RIOT 10）：多了或漏了边界检查。

QSemOS 典型：
| 偏差类型 | 示例 | 后果 | 任务 |
|----------|------|------|:---:|
| 多加了守卫 | `usedSize > 0` 才调 flush hook | 空 buffer 时 hook 不触发 | 4 (DS) |
| 漏了守卫 | 没检查 `OS_TSK_LOCK` 就阻塞 | 持有锁进入等待 → 死锁 | 84 (GLM) |
| 检查顺序错 | 先测 `OS_INT_ACTIVE` 再验参数 | 参数错误码被上下文错误码覆盖 | 224 (GLM) |

RIOT 典型：7 例 NULL/边界检查缺失导致崩溃——`ipv6_addr_match_prefix` 未检查 NULL、`fmt_hex_bytes` 奇数长度越界、`gnrc_pktqueue_remove` 未检查 queue/node。另有 3 例条件判断偏差导致测试失败。

**步骤不完整**（QSemOS 23 / RIOT 4）：多步操作只做了一部分，漏掉了前后关联的步骤。

QSemOS 典型——spinlock irqsave 四步协议：
```
模型写的:  (void)spinLock;                     ← 以为 spinlock 只是一个函数调用
实际要做:  OsSplLock(spinLock)                  ← 锁硬件
          + OS_TSK_SPINLOCK()                  ← 标记任务不可抢占
          + OsIntLock() / OsIntRestore(intSave) ← 关中断配对（intSave 必须透传）
```
通过率 20%（tasks 151/152/153/221）。

信号状态机三字段联动——`sigPending` + `sigWaitMask` + `sigInfoList` 描述同一个等待关系，每次操作必须三者联动：

| 函数 | 做了什么 | 漏了什么 | 任务 |
|------|---------|---------|:---:|
| `OsSigWaitProcPendingSignal` | 删链表节点、清 pending bit、释放内存 | `sigWaitMask = 0` | 80 (DS+GLM) |
| `OsSignalTimeOutSet` | `TSK_STATUS_SET(TIMEOUT)` | `else { TSK_STATUS_CLEAR(TIMEOUT) }` | 82 (DS+GLM) |
| `OsSignalWaitSche` | 开始阻塞流程 | `TSK_STATUS_TST(OS_TSK_LOCK)` 锁前检查 | 84 (GLM) |

RIOT 典型：4 例网络协议栈指针操作——`gnrc_netreg_getnext`、`gnrc_pktbuf_mark` 等需要多步协议栈操作，模型只做了其中一步。

**逻辑错误**（QSemOS 10 / RIOT 11）：算法方向、位运算、计算公式本身出错。

QSemOS 典型：
| 函数 | 模型做的 | 实际应做的 | 任务 |
|------|---------|-----------|:---:|
| `OsPerfTaskFilter` | 匹配 = TRUE（白名单，保留匹配项） | 匹配 = FALSE（黑名单，过滤匹配项） | 114 (GLM) |
| `PRT_SignalMask` | 先 `sigMask &= ~*set` 再 `OsHandleUnBlockSignal` | 先 deliver 再清 mask | 88 (DS+GLM) |
| `OsRingbufWriteLinear` | 可用空间 = `startIdx - endIdx` | `startIdx <= endIdx` 时 wraparound 方向反 | 92 (DS+GLM) |

RIOT 典型：11 例算法/逻辑错误，分散在 17 种子类——运算符错误、循环逻辑错误、累加器精度、数据结构顺序等，反映通用编程能力的随机失误，与 QSemOS 的系统性偏差形成对比。

**表示误读**（QSemOS 8 / RIOT 4）：把数据的一种表示方式当成了另一种——定制编码当标准格式、偏移量当绝对值。

| 模型的理解 | 实际约定 | 后果 | 任务 |
|-----------|---------|------|:---:|
| `expectedTick` 是绝对时间戳 → 直接减法 | 链表中相对上一节点的 roll-number 偏移量 | 通过率 0% | 66 (DS) |
| `state` 是简单枚举 → 直接赋值 | 复合位掩码（低几位生命周期 + 高位配置 flag） | 覆盖了定时器周期/自动重载 flag | 72/132 (GLM) |

RIOT 典型：部分算法/逻辑错误源于对 RIOT 内部数据结构编码的误读——将自定义的消息队列格式或定时器编码当作标准结构处理。

**上下文错配**（QSemOS 8 / RIOT 5）：在应走 A 路径的地方走了 B 路径，或调用了不适合当前状态的 API。

| 函数 | 模型用的 | 实际需要的 | 任务 |
|------|---------|-----------|:---:|
| `OsFirstTimeSwitch` | 热切换逻辑（保存当前上下文→选下一任务→恢复） | 冷启动（无当前任务，直接 `OsTskContextLoad`） | 141 (DS+GLM) |
| `OsTskEntry` | 直接 `RUNNING_TASK->taskEntry()` | 需 `RUNNING_TASK` 空指针保护 | 191 (GLM) |
| `OsHwiHookDispatcher` | 编造 `OsHwiFuncGet` 手动派发 | 调用内核标准 `OsHwiHandleActive` | 104 (DS) |

RIOT 典型：5 例架构/API 误用——选错了中断注册路径、调用了不适合当前上下文的 API，与 QSemOS 的路径混淆同源。

### 5.3 上下文不可获取（4 例）

正确答案不在提供的源文件里，模型只能猜。全部是字符串字面量。

| 函数 | 模型猜的 | 正确答案 | 任务 |
|------|---------|---------|:---:|
| `OsPerfDefaultNotify` | `"perf buf water mark!"` | `"perf buf waterline notify!"` | 1 (GLM) |
| `OsPerfGetEventName` | `"cpu-clock"` | `"timed"` | 133 (GLM) |

### 5.4 分布总览

**模型幻觉**（编译失败）

| 子类 | 描述 | QSemOS-DS | QSemOS-GLM | RIOT-DS |
|------|------|:---:|:---:|:---:|
| 对称命名推理 | 看到 `XxxGet` 就假设存在 `XxxSet`，按命名对称编造反向 API | 2 | 2 | 6 |
| 功能推断编造 | 为完成当前功能凭空编了一组不存在的辅助函数 | 3 | 4 | ~2 |
| 符号环境错配 | 调用了训练数据中的 POSIX/C11 符号，嵌入式编译链不提供 | 2 | 6 | 0 |
| 命名猜测 | 知道需要某个语义但猜错了项目里的实际名字 | 1 | 3 | ~2 |
| **小计** | | **8** | **15** | **~10** |

> 符号环境错配在 RIOT 为零——开源项目头文件全部可见，模型不会错误假设 POSIX 符号存在。这是闭源 benchmark 独有的失败模式。

**模型理解能力**（测试失败 + 崩溃）

| 子类 | 描述 | QSemOS-DS | QSemOS-GLM | RIOT-DS |
|------|------|:---:|:---:|:---:|
| 条件判断偏差 | 多了或漏了边界检查：NULL 判断、范围校验、状态前置条件 | 6 | 5 | 10 |
| 步骤不完整 | 多步操作只做了一部分，漏掉了前后关联的步骤 | 10 | 13 | 4 |
| 逻辑错误 | 算法方向、位运算、计算公式本身出错 | 5 | 5 | 11 |
| 表示误读 | 把数据的一种表示方式当成了另一种：定制编码当标准格式、偏移量当绝对值 | 3 | 5 | 4 |
| 上下文错配 | 在应走 A 路径的地方走了 B 路径，或调用了不适合当前状态的 API | 4 | 4 | 5 |
| **小计** | | **28** | **32** | **~34** |

> 条件判断偏差两套 benchmark 都大量存在，是最稳定的跨基准失败模式。步骤不完整在 QSemOS 最突出（23 例）——闭源内核的多步同步协议（spinlock irqsave 四步、signal 三字段联动）模型很难一次写全。逻辑错误在 RIOT 最突出（11 例）——分散在 17 种子类（运算符、循环、累加器精度），是通用编程能力的随机失误。
>
> RIOT 另有 4 例 `-Werror` 编译失败不在上述分类中：2 例去掉 `-Werror` 即可通过（代码逻辑正确，构建配置不兼容），2 例去掉后暴露深层的逻辑/路径错误（分别归入逻辑错误和上下文错配）。-Werror 属于构建环境差异，非模型代码生成质量的直接反映。
>
> **上下文不可获取**：QSemOS 4 例（DS 1 / GLM 3），RIOT ~3 例，全部为字符串字面量。

### 5.5 OS 模块架构

QSemOS 和 RIOT 的功能模块覆盖存在系统性差异。两套 benchmark 涉及的模块：

| 功能域 | QSemOS 模块 | QSemOS 通过率 | RIOT 模块 | RIOT 通过率 |
|--------|------------|:---:|------|:---:|
| **任务调度** | kernel/task, kernel/sched, kernel/task_init | ~80% | core/thread | ~94% |
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

QSemOS 独有的模块（信号、读写锁、自旋锁、动态模块加载、性能监控）集中在 **内核同步** 和 **性能可观测性** 两个方向，这正是模型理解能力短板最集中的区域。RIOT 独有的模块（内存管理、文件系统、网络协议栈）在 QSemOS 上无对应测试，模型在这些模块的表现未知。

### 5.6 越界编辑

chmod 444 + tree-sitter mask 机制约束了编辑范围。GLM 5 例、DS 1 例修改了 mask 区域外的代码。这不属于代码生成质量的失败根因——越界了的任务测试可能通过也可能失败，但说明 GLM 更倾向于在工具约束边界试探。
