# RAG 实验结果分析报告

## 1. 总体恢复率

| 实验             | 基线失败 | RAG通过 | RAG失败 | 恢复率   | 基线通过率 | RAG后总通过率 |
| -------------- | ---- | ----- | ----- | ----- | ----- | -------- |
| RIOT-DS-RAG    | 50   | 18    | 32    | 36.0% | 90.7% | 94.1%    |
| RIOT-GLM-RAG   | 63   | 30    | 33    | 47.6% | 88.3% | 93.9%    |
| QSemOS-DS-RAG  | 40   | 27    | 13    | 67.5% | 83.1% | 94.5%    |
| QSemOS-GLM-RAG | 44   | 30    | 14    | 68.2% | 81.4% | 94.1%    |

## 2. 按错误类型的恢复率

### RIOT

| 错误类型                       | 基线失败数 | RAG通过 | RAG仍失败 | 恢复率  |
| -------------------------- | ----- | ----- | ------ | ---- |
| DS test\_failure           | 19    | 5     | 14     | 26%  |
| DS compile\_error          | 14    | 6     | 8      | 43%  |
| DS test\_not\_executed     | 7     | 2     | 5      | 29%  |
| DS illegal\_modifications  | 6     | 5     | 1      | 83%  |
| DS crash                   | 4     | 0     | 4      | 0%   |
| GLM compile\_error         | 25    | 13    | 12     | 52%  |
| GLM test\_failure          | 23    | 7     | 16     | 30%  |
| GLM test\_not\_executed    | 6     | 2     | 3      | 33%  |
| GLM illegal\_modifications | 5     | 5     | 0      | 100% |
| GLM crash                  | 4     | 3     | 1      | 75%  |

### QSemOS

| 错误类型               | 基线失败数 | RAG通过 | RAG仍失败 | 恢复率 |
| ------------------ | ----- | ----- | ------ | --- |
| DS test\_failure   | 22    | 15    | 7      | 68% |
| DS compile\_error  | 10    | 6     | 4      | 60% |
| DS crash           | 8     | 6     | 2      | 75% |
| GLM test\_failure  | 31    | 20    | 11     | 65% |
| GLM compile\_error | 13    | 10    | 3      | 77% |

## 3. RAG 失败的错误分布

| 实验             | test\_failure | compile\_error | test\_not\_executed | crash | illegal\_modifications |
| -------------- | ------------- | -------------- | ------------------- | ----- | ---------------------- |
| RIOT-DS-RAG    | 14            | 8              | 5                   | 4     | 1                      |
| RIOT-GLM-RAG   | 16            | 13             | 3                   | 1     | 0                      |
| QSemOS-DS-RAG  | 7             | 4              | 0                   | 2     | 0                      |
| QSemOS-GLM-RAG | 11            | 3              | 0                   | 0     | 0                      |

## 4. 完整性统计

| 实验             | Clean Pass | Dirty Pass | Integrity OK |
| -------------- | ---------- | ---------- | ------------ |
| RIOT-DS-RAG    | 15         | 3          | 47/50 (94%)  |
| RIOT-GLM-RAG   | 27         | 3          | 60/63 (95%)  |
| QSemOS-DS-RAG  | 27         | 0          | 40/40 (100%) |
| QSemOS-GLM-RAG | 29         | 1          | 43/44 (98%)  |

## 5. 发现

1. **QSemOS(闭源)的RAG收益远超RIOT(开源)**：QSemOS恢复率\~68% vs RIOT\~42%。闭源OS的API签名和类型不在模型训练数据中，RAG补齐后效果显著。
2. **GLM比DS更受益于RAG**：RIOT上GLM恢复率(47.6%)比DS(36.0%)高11.6pp，QSemOS上两者接近。GLM在缺少上下文时更容易幻觉，RAG有效抑制了这一问题。
3. **compile\_error 对 RAG 有抵抗力**：RAG后仍有25-42%的失败是编译错误，说明当前检索上下文对类型系统/宏展开的覆盖不足。
4. **QSemOS RAG 完整性 100%**：闭源场景下 RAG 上下文未诱发越界编辑，DS 的 40 个任务零 dirty。
5. **Dirty Pass 集中在 RIOT 的 3 个任务**：task\_151、203、491 在 DS 和 GLM 两侧都触发越界编辑，是 RAG 上下文片段导致模型编辑了目标函数外的代码。

## 6. RIOT test_not_executed 场景分析

根因：harness 只认 QSemOS 的 `Passes: N / Failures: M` 格式，不认识 RIOT 的 `run N failures M` 格式，导致只要是 ASan 崩溃退出或 RIOT 格式输出的都被误判为"测试未执行"。

### 基线

**DS 基线（7 个，全是 ASan 崩溃）：**

| Task | 函数 | 实际场景 | 点数 | 崩溃位置 |
|------|------|----------|:---:|------|
| 28 | `bluetil_addr_ipv6_l2ll_sprint` | 首测即崩 | 0 | 测试框架 `assertImplementationCStr`（栈溢出） |
| 38 | `coap_build_reply_header` | 34 个通过后崩 | 34 | 补全代码 NULL 解引用 |
| 47 | `coap_opt_add_uri_query2` | 12 个通过后崩 | 12 | 补全代码 |
| 128 | `fmt_hex_bytes` | 6 个通过后崩 | 6 | 补全代码 |
| 206 | `ipv6_addr_from_str` | 79 个通过后崩 | 79 | 补全代码 |
| 209 | `ipv6_addr_match_prefix` | 38 个通过后崩 | 38 | 补全代码 NULL 解引用 |
| 507 | `gnrc_pktqueue_remove` | 3 个通过后崩 | 3 | 补全代码 NULL 解引用 |

**GLM 基线（6 个，另 task_197/207 已修正为 test_failure）：**

| Task | 实际场景 | 详情 |
|------|----------|------|
| 28 | 首测即崩 | 同 DS |
| 38 | 34 通过后崩 | 同 DS |
| 53 | 14 通过后崩 | ASan |
| 201 | 19 通过后崩 | ASan |
| 209 | 38 通过后崩 | ASan |
| 507 | 3 通过后崩 | ASan |

### RAG

**DS-RAG（5 个，全部 ASan 崩溃）：**

| Task | 实际场景 | 点数 |
|------|----------|:---:|
| 28 | 首测即崩 | 0 |
| 38 | 34 通过后崩 | 34 |
| 47 | 12 通过后崩 | 12 |
| 209 | 38 通过后崩 | 38 |
| 507 | 3 通过后崩 | 3 |

**GLM-RAG（4 个，另 task_197/207 已修正为 test_failure）：**

| Task | 实际场景 | 详情 |
|------|----------|------|
| 28 | 首测即崩 | ASan |
| 201 | 编译失败 | 无点号，编译阶段就挂了 |
| 209 | 38 通过后崩 | ASan |
| 507 | 3 通过后崩 | ASan |

### 结论

`test_not_executed` 这个分类名不准确，实际是两种场景：

1. **ASan/SIGSEGV 崩溃（多数）**：测试跑了部分后崩溃（有点号输出），应归为 `crash`
2. **harness 解析失败**：测试正常跑完输出 `run N failures M`，harness 不认识此格式。典型 case 为 task_197/207（已修正为 `test_failure`）

task_28 稍特殊——崩溃在测试框架内部（`assertImplementationCStr` 栈溢出），不是补全代码的问题，但同样属于 crash。

