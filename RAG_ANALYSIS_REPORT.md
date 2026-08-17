# RAG 实验结果分析报告

## 1. 总体恢复率

| 实验             | 基线失败 | RAG通过 | RAG失败 | 恢复率   | 基线通过率 | RAG后总通过率 |
| -------------- | ---- | ----- | ----- | ----- | ----- | -------- |
| RIOT-DS-RAG    | 50   | 18    | 32    | 36.0% | 90.7% | 94.1%    |
| RIOT-GLM-RAG   | 63   | 31    | 32    | 49.2% | 88.3% | 94.1%    |
| QSemOS-DS-RAG  | 39   | 21    | 18    | 53.8% | 83.5% | 92.4%    |
| QSemOS-GLM-RAG | 43   | 26    | 17    | 60.5% | 81.9% | 92.8%    |

## 2. 按错误类型的恢复率

### RIOT

| 错误类型                       | 基线失败数 | RAG通过 | RAG仍失败 | 恢复率  |
| -------------------------- | ----- | ----- | ------ | ---- |
| DS test\_failure           | 19    | 4     | 15     | 21%  |
| DS compile\_error          | 14    | 6     | 8      | 43%  |
| DS test\_not\_executed     | 7     | 3     | 4      | 43%  |
| DS illegal\_modifications  | 6     | 4     | 2      | 67%  |
| DS crash                   | 4     | 1     | 3      | 25%  |
| GLM compile\_error         | 25    | 15    | 10     | 60%  |
| GLM test\_failure          | 21    | 9     | 12     | 43%  |
| GLM test\_not\_executed    | 8     | 1     | 7      | 12%  |
| GLM illegal\_modifications | 5     | 4     | 1      | 80%  |
| GLM crash                  | 4     | 2     | 2      | 50%  |

### QSemOS

| 错误类型               | 基线失败数 | RAG通过 | RAG仍失败 | 恢复率 |
| ------------------ | ----- | ----- | ------ | --- |
| DS test\_failure   | 21    | 10    | 11     | 48% |
| DS compile\_error  | 10    | 7     | 3      | 70% |
| DS crash           | 8     | 4     | 4      | 50% |
| GLM test\_failure  | 30    | 17    | 13     | 57% |
| GLM compile\_error | 13    | 9     | 4      | 69% |

## 3. RAG 失败的错误分布

| 实验             | test\_failure | compile\_error | test\_not\_executed | crash | illegal\_modifications |
| -------------- | ------------- | -------------- | ------------------- | ----- | ---------------------- |
| RIOT-DS-RAG    | 15            | 9              | 5                   | 3     | 0                      |
| RIOT-GLM-RAG   | 14            | 11             | 5                   | 2     | 0                      |
| QSemOS-DS-RAG  | 15            | 3              | 0                   | 0     | 0                      |
| QSemOS-GLM-RAG | 14            | 3              | 0                   | 0     | 0                      |

## 4. 完整性统计

| 实验             | Clean Pass | Dirty Pass | Integrity OK |
| -------------- | ---------- | ---------- | ------------ |
| RIOT-DS-RAG    | 14         | 4          | 46/50 (92%)  |
| RIOT-GLM-RAG   | 29         | 2          | 61/63 (97%)  |
| QSemOS-DS-RAG  | 21         | 0          | 39/39 (100%) |
| QSemOS-GLM-RAG | 26         | 0          | 43/43 (100%) |

## 5. 发现

1. **QSemOS(闭源)的RAG收益远超RIOT(开源)**：QSemOS恢复率\~57% vs RIOT\~43%。闭源OS的API签名和类型不在模型训练数据中，RAG补齐后效果显著。
2. **GLM比DS更受益于RAG**：RIOT上GLM恢复率(49.2%)比DS(36.0%)高13.2pp，QSemOS上GLM(60.5%)比DS(53.8%)高6.6pp。GLM在缺少上下文时更容易幻觉，RAG有效抑制了这一问题。
3. **compile\_error 对 RAG 有抵抗力**：RAG后仍有14-34%的失败是编译错误，说明当前检索上下文对类型系统/宏展开的覆盖不足。
4. **QSemOS RAG 完整性 100%**：闭源场景下 RAG 上下文未诱发越界编辑，两个模型均零 dirty pass。
5. **Dirty Pass 仅出现在 RIOT**：开源场景下模型更容易受 RAG 上下文片段诱导编辑非目标代码。

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

