# QSem-Claude

Claude Code 驱动的闭源嵌入式操作系统内核函数补全 benchmark。模型看源码 + 头文件，补全被 mask 的函数体，编译跑测试。

**数据集**: 240 个 OS 内核函数，覆盖任务调度、中断、IPC、同步、定时器、性能监控、动态模块加载 7 个功能域。
**对比基准**: RIOT OS（开源 IoT 操作系统，538 个函数）。

## 结果

| 模型 | QSemOS 通过率 | RIOT 通过率 |
|------|:---:|:---:|
| DS-v4-pro | **82.9%** (199/240) | 90.7% (488/538) |
| GLM-5.1 | **80.8%** (194/240) | — |
| kimi-k2.7-code | **95.8%** (23/24)* | — |

> *kimi 仅完成前 24 个任务，数据仅供参考。

DS-v4-pro 在 QSemOS（闭源）上的通过率比 RIOT（开源）低 7.8 个百分点——头文件和 API 文档不可见直接拉低了模型表现。

## 失败根因分类

QSemOS（87 例失败）+ RIOT（44 例失败，排除完整性违规）归入三个大类。

### 一、模型幻觉（QSemOS 23 / RIOT ~10，全部编译失败）

模型编造了不存在的符号，链接阶段报 `undefined reference`。

| 子类 | 描述 | QSemOS | RIOT |
|------|------|:---:|:---:|
| **对称命名推理** | 看到 `XxxSet` 就假设存在 `XxxGet`，按命名对称编造反向 API | 4 | 6 |
| **功能推断编造** | 为完成当前功能凭空编了一组不存在的辅助函数 | 7 | ~2 |
| **符号环境错配** | 调用了训练数据中的 POSIX/C11 符号，嵌入式编译链不提供 | 8 | 0 |
| **命名猜测** | 知道需要某个语义但猜错了项目里的实际名字 | 4 | ~2 |

**典型 case**:

```
模型写:  memcpy_s(dst, len, src, len);    // C11 Annex K，嵌入式 toolchain 不提供
实际应:  memcpy(dst, src, len);            // 标准 C 库版本

模型写:  OsHwiFuncGet(hwiNum);             // 看到 OsHwiFuncSet → 假设存在 Get
实际应:  不存在此 API，需用内核派发路径

模型写:  OsSemPendParaCheck() + OsSemPendNotNeedSche() + OsSemPendListPut()
实际应:  这三个函数都不存在，是模型根据"信号量阻塞"语义逐层编的
```

符号环境错配在 RIOT 为零——开源项目所有头文件可见，模型不需要猜。

### 二、模型理解能力（QSemOS 60 / RIOT ~34，测试失败 + 崩溃）

代码能编译，但逻辑有偏差。五个子类按抽象程度排列：

| 子类 | 一句话 | QSemOS | RIOT |
|------|------|:---:|:---:|
| **条件判断偏差** | 多了或漏了边界检查 | 11 | 10 |
| **步骤不完整** | 多步操作只做了一部分，漏掉了前后关联的步骤 | 23 | 4 |
| **逻辑错误** | 算法方向、位运算、计算公式本身出错 | 10 | 11 |
| **表示误读** | 把数据的一种表示方式当成了另一种 | 8 | 4 |
| **上下文错配** | 在应走 A 路径的地方走了 B 路径 | 8 | 5 |

**条件判断偏差**——两套 benchmark 最稳定的失败模式：

```
// 漏了守卫 — 持有锁进入等待 → 死锁
if (mask & OS_SIGNAL_MATCH) { OsSigWaitSche(); }  // 没检查 OS_TSK_LOCK

// 多加了守卫 — 空 buffer 时 hook 不触发
if (usedSize > 0) { flushHook(); }  // usedSize == 0 也应该触发

// RIOT 同类：ipv6_addr_match_prefix(prefix, addr) — 没检查 prefix == NULL → 崩溃
```

**步骤不完整**——QSemOS 特有倾斜（23 例 vs RIOT 4 例），根源是闭源内核的同步原语：

```
// spinlock irqsave 四步协议，模型只写了一步
(void)spinLock;                          // ← 模型写的（空壳）

OsSplLock(spinLock);                     // 1. 锁硬件
OS_TSK_SPINLOCK();                       // 2. 标记任务不可抢占
intSave = OsIntLock();                   // 3. 关中断
// ... 临界区 ...
OsIntRestore(intSave);                   // 4. 恢复中断（intSave 必须透传）

// signal 状态机三字段联动
sigPending  = who_is_waiting             // 改了
sigInfoList = malloc(...)                // 改了
sigWaitMask = 0                          // 漏了 — 任务永远等不到信号
```

**逻辑错误**——RIOT 特有倾斜（11 例 vs QSemOS 10 例）。RIOT 的 11 例分散在 17 种子类（运算符错、循环逻辑错、累加器精度、数据结构顺序），是通用编程能力的随机失误。QSemOS 的 10 例更集中——白名单 vs 黑名单反转、wraparound 方向算反、信号 deliver 顺序反了。

**表示误读**——模型把项目定制的编码格式当成了标准格式：

| 模型的理解 | 实际约定 | 后果 |
|-----------|---------|------|
| `expectedTick` 是绝对时间戳 | 链表中相对上一节点的 roll-number 偏移量 | 通过率 0% |
| `state` 是简单枚举，直接 `=` 赋值 | 复合位掩码（低几位生命周期 + 高位配置 flag） | 覆盖了定时器自动重载 flag |

**上下文错配**——冷启动用了热切换逻辑、中断派发绕过了内核标准路径 `OsHwiHandleActive`。RIOT 同类：选错中断注册路径、在中断上下文调了阻塞 API。

### 三、上下文不可获取（QSemOS 4 / RIOT ~3）

正确答案不在提供的源文件里，全部是字符串字面量。`"perf buf waterline notify!"` vs `"water mark!"`——模型看不见真实的 printf 格式串，只能从函数名猜。

### RIOT -Werror（4 例，独立统计）

2 例去掉 `-Werror` 即可通过（代码逻辑正确，构建配置不兼容），2 例去掉后暴露深层的逻辑/路径错误。`-Werror` 属于构建环境差异，非代码生成质量本身的问题。

### 越界编辑（6 例，独立统计）

GLM 5 例、DS 1 例修改了 tree-sitter mask 区域外的代码。不影响通过/失败判定，但反映 GLM 更倾向在工具约束边界试探。

## OS 模块架构

QSemOS 和 RIOT 的功能模块覆盖存在系统差异：

| 功能域 | QSemOS 模块（通过率） | RIOT 模块（通过率） |
|--------|:---:|:---:|
| 任务调度 | kernel/task (80%) | core/thread (94%) |
| 中断管理 | kernel/irq (89%) | core/irq (92%) |
| IPC 信号量 | ipc/sem (86%) | sys/sem (88%) |
| IPC 消息队列 | ipc/queue (85%) | core/msg (86%) |
| 定时器 | kernel/timer (89%) | sys/xtimer (86%) |
| 日志 | utility/log (85%) | sys/log (89%) |
| **IPC 信号** | **ipc/signal (73%)** | — |
| **IPC 读写锁** | **ipc/rwlock (100%)** | — |
| **自旋锁** | **kernel/spinlock (67%)** | — |
| **动态模块** | **extend/dlmodule (91%)** | — |
| **性能监控** | **extended/perf (75%)** | — |
| 互斥锁 | — | sys/mutex (91%) |
| 内存管理 | — | core/mem (93%) |
| 文件系统 | — | sys/vfs (86%) |
| 网络协议栈 | — | net/gnrc (79%) |

QSemOS 独有的 5 个模块集中在内核同步（信号、读写锁、自旋锁）和性能可观测性（perf、dlmodule），正是模型理解能力短板最集中的区域——spinlock 通过率 66.7%、signal 通过率 73.3%。RIOT 独有的内存管理、文件系统、网络协议栈在 QSemOS 上无对应测试。

## 项目结构

```
qsem-claude/
├── BENCHMARK_REPORT_V2.md         # QSemOS 240-task 全量报告（多模型）
├── CROSS_BENCHMARK_ANALYSIS.md    # QSemOS × RIOT 跨基准对比分析
├── README_NEW.md                  # 本文件
│
├── config.py                      # 路径、超时、编译配置
├── runner.py                      # 主循环：逐 task 执行 12 步流程
├── agent.py                       # Claude Code CLI 交互（stdin pipe）
├── mask.py                        # tree-sitter AST 解析 + 函数体替换
├── prompt.py                      # 给 Claude 的 system prompt
├── logger.py                      # 轨迹日志写入
├── git_utils.py                   # git 状态检查（继承自 riot-claude）
│
├── analyze_results.py             # 结果统计脚本
├── whitebox_analysis.py           # DS-v4-pro 失败根因自动分类
├── whitebox_glm.py                # GLM-5.1 失败根因自动分类
├── whitebox_riot.py               # RIOT 失败根因自动分类
│
├── results/
│   ├── deepseek-v4-pro/results.jsonl   # 240 tasks
│   ├── glm-5.1/results.jsonl           # 240 tasks
│   └── kimi-k2.7-code/results.jsonl    # 24 tasks
│
└── trajectory/
    ├── deepseek-v4-pro/task_*.log      # 每个 task 的完整 Claude 交互日志
    ├── glm-5.1/task_*.log
    └── kimi-k2.7-code/task_*.log
```

## 执行流程

每个 task 执行 12 步：

```
1. 备份源文件         → 内存变量
2. 备份 func_under_test.c → 内存变量（ground truth）
3. 锁定权限           → chmod 444 所有源文件，仅目标文件 chmod u+w
4. Mask 函数体        → tree-sitter 解析 AST，替换 body 为 /* MASKED */
5. 环境快照           → model/CLI/prompt/dataset hash
6. 隐藏 check_tests   → mv check_tests → .check_tests_hidden
   ├── 启动 Claude     → stdin pipe，cwd=QSemOS/
   ├── 监控输出         → 检测 hook 标记 + watchdog（8h 无输出→kill）
   └── Claude 退出
7. 恢复 check_tests   → mv .check_tests_hidden → check_tests（finally 块保证）
8. 完整性检查         → 归一化函数签名，比较 masked 版和编辑版
9. 提取函数体         → tree-sitter 提取，注入 func_under_test.c
10. 编译运行测试       → cd check_tests/tasks/task_NNN && make && ./test
11. 记录结果           → results.jsonl + trajectory/task_N.log
12. 恢复文件           → 源文件 + func_under_test.c（finally 块保证）
```

## 使用方法

```bash
cd QSemOS/qsem-claude

# 运行单个 task
conda run -n base python runner.py --task-id 1

# 批量运行
conda run -n base python runner.py --batch 240 --resume

# 分析结果
conda run -n base python analyze_results.py
```

## 与 RIOT benchmark 的区别

| | QSemOS | RIOT |
|------|------|------|
| 评估对象 | 闭源嵌入式 OS kernel | 开源 IoT OS |
| 任务数 | 240 | 538 |
| 模型可见 API 文档 | 无（头文件需推断） | 全部可见 |
| 隔离方式 | chmod 444 + tree-sitter mask | Docker + git reset |
| 内存管理/文件系统/网络 | 不涉及 | 涉及 |
| 内核同步原语 | spinlock, signal, rwlock | mutex |
| 主要失败模式 | 步骤不完整（同步协议缺步） | 条件判断偏差（NULL/边界缺失） |
