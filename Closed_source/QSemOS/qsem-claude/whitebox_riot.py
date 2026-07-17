"""
RIOT White-Box Failure Analysis
================================
对 RIOT benchmark (DeepSeek-v4-pro) 的 50 个失败任务做三级分类，
重点分析 19 个 test_failure 的 diff-vs-oracle 根因。
"""
import json
import re
from pathlib import Path
from collections import Counter

RIOT_ROOT = Path("//wsl.localhost/Ubuntu/home/carl/RIOT")
RESULTS_FILE = RIOT_ROOT / "riot-claude/results/results_deepseek-v4-pro.jsonl"
TRAJ_DIR = RIOT_ROOT / "riot-claude/trajectory/results_deepseek-v4-pro"

L1_LABELS = {
    "compile_error": "编译失败",
    "crash": "运行崩溃",
    "test_failure": "测试失败",
    "test_not_executed": "测试未执行",
    "illegal_modifications": "完整性违规",
}

# ============================================================
# L2/L3 分类 — 基于对 diff + test output + oracle 的手动分析
# ============================================================

# 19 个 test_failure 的手动三级分类
TEST_FAILURE_CLASSIFICATION = {
    1: {
        "function": "adc_util_map",
        "module": "sys/analog_util",
        "L2": "wrong_operator",
        "L3": "用 / 代替 >> 导致有符号整数舍入方向错误 (16062 vs 16063)",
        "detail": "Oracle: (((int64_t)(max-min)*sample) >> _adc_res_bits(res)) + min; Claude: / 替代 >> 导致向零舍入而非向下舍入",
    },
    37: {
        "function": "coap_build_reply",
        "module": "sys/net/nanocoap",
        "L2": "semantic_misunderstanding",
        "L3": "返回值语义错误：返回 hdr_len+payload_len 而非仅 hdr_len (exp 4 was 8)",
        "detail": "Oracle 只构建 header 并返回 coap_hdr_len; Claude 多加了 payload_len",
    },
    58: {
        "function": "coding_xor_recover",
        "module": "sys/coding",
        "L2": "algorithm_incomplete",
        "L3": "恢复算法不完整：输出 '0' 而非 '0123456789AB'",
        "detail": "调用了 _unmix 和 _recover_blocks 但恢复逻辑不完整; parity 恢复分支也导致 secondary failure (exp 192 was 67)",
    },
    67: {
        "function": "crb_end_chunk",
        "module": "sys/chunked_ringbuffer",
        "L2": "side_effect_error",
        "L3": "chunk 管理状态机错误：断言失败 (run 2 failures 1)",
        "detail": "Claude 实现了 _get_free_chunk 分配逻辑，但 oracle 的引用计数/状态管理不同; keep=false 时返回值也不正确",
    },
    185: {
        "function": "gnrc_sixlowpan_frag_vrb_add",
        "module": "sys/net/gnrc/sixlowpan/frag",
        "L2": "missing_corner_cases",
        "L3": "VRB 添加逻辑缺少 tag 冲突检测和重复条目处理 (exp 1742197326 was 343)",
        "detail": "Claude 只查找空槽位并填充，Oracle 还需处理: tag collision, duplicate datagram, 超时条目回收",
    },
    196: {
        "function": "ieee802154_get_frame_hdr_len",
        "module": "sys/net/ieee802154",
        "L2": "control_flow_error",
        "L3": "PAN compression 条件判断逻辑错误 (exp 0 was 3)",
        "detail": "Claude 对 FCF_PAN_COMP 和 DST_ADDR_VOID 的组合判断与 IEEE 802.15.4 标准不一致",
    },
    198: {
        "function": "ieee802154_set_frame_hdr",
        "module": "sys/net/ieee802154",
        "L2": "algorithm_incomplete",
        "L3": "帧头构造系统性偏差：4 个测试全部失败 (exp 3/7/11/17 was 0/3/7/13)",
        "detail": "109 行实现但 offset 计算/地址写入顺序与 Oracle 不同; 缺少 PAN ID compression 的正确处理",
    },
    199: {
        "function": "inet_csum_slice",
        "module": "sys/net/inet_csum",
        "L2": "accumulator_precision",
        "L3": "校验和累加器精度不足 + 奇偶处理偏差 (全部 8 个测试 off-by-2~7)",
        "detail": "Oracle 用 uint32_t csum 累加; Claude 用 uint16_t 参数直接操作; 奇字节处理逻辑不同 (accum_len+len vs len)",
    },
    207: {
        "function": "ipv6_addr_init_iid",
        "module": "sys/net/ipv6/addr",
        "L2": "bit_manipulation_error",
        "L3": "IID 位操作错误 (exp 1 was 0)",
        "detail": "函数为 void 但测试检查输出结构体字段; Claude 的 bit shift/mask 逻辑与 Oracle 不同",
    },
    213: {
        "function": "isrpipe_read_all_timeout",
        "module": "sys/isrpipe",
        "L2": "architecture_error",
        "L3": "重新实现底层逻辑而非复用 isrpipe_read_timeout (exp -110 was 1)",
        "detail": "Oracle: 简单循环调用 isrpipe_read_timeout(); Claude: 手动操作 ztimer/mutex/tsrb，返回了错误的超时码",
    },
    227: {
        "function": "mtd_erase",
        "module": "drivers/mtd",
        "L2": "missing_conditional_guard",
        "L3": "缺少 addr/count 对齐检查 + 缺少 driver->erase 直接路径 (exp -75 was 0)",
        "detail": "Oracle 有 count % sector_size 和 addr % sector_size 检查返回 -EOVERFLOW; Claude 直接除然后调 mtd_erase_sector",
    },
    247: {
        "function": "rtc_localtime",
        "module": "sys/rtc_utils",
        "L2": "loop_logic_error",
        "L3": "年/月循环计算错误：tm_hour/tm_min/tm_sec 全为 0 (exp 3/1/5 was 0)",
        "detail": "Claude 从零开始重新实现时间转换; 闰年/月长计算循环有 bug 导致 time 变量被错误消耗",
    },
    255: {
        "function": "saul_reg_add",
        "module": "sys/saul_reg",
        "L2": "data_structure_order",
        "L3": "链表头插而非尾插 (exp 'S0' was 'S1')",
        "detail": "Oracle: 遍历到链表尾部追加 (FIFO); Claude: dev->next = saul_reg; saul_reg = dev (LIFO prepend)",
    },
    294: {
        "function": "strscpy",
        "module": "sys/libc",
        "L2": "mixed_guard_and_loop",
        "L3": "缺少 count==0 提前检查 + 循环边界与 Oracle 不同 (exp 5 was -7)",
        "detail": "Oracle: if (!count) return -E2BIG; while (--count && *src); Claude: for (i=0; i<count-1 && src[i]; i++) 导致 count<=strlen 时错误返回 -E2BIG",
    },
    347: {
        "function": "uri_parser_split_query",
        "module": "sys/uri_parser",
        "L2": "validation_logic_inverted",
        "L3": "空段检测和错误返回逻辑反转 (exp -1 was 2; exp 3 was -1)",
        "detail": "Claude 在遇到空段时返回 -1 但 oracle 容忍某些空段; 成功路径上 Claude 又过早返回错误",
    },
    349: {
        "function": "uuid_to_string",
        "module": "sys/uuid",
        "L2": "wrong_return_value",
        "L3": "UUID 格式化返回值错误 (exp 0 was 32)",
        "detail": "函数为 void 但测试通过 embUnit 检查输出; 可能 fmt_byte_hex 返回值累积方式与 Oracle 不同",
    },
    355: {
        "function": "vfs_closedir",
        "module": "sys/vfs",
        "L2": "missing_conditional_guard",
        "L3": "缺少 mountp==NULL → -EBADF 检查 + 缺少 memset 清零 (exp -9 was 0)",
        "detail": "Oracle: if (mountp==NULL) return -EBADF; memset(dirp,0,sizeof(*dirp)); Claude: 跳过 mountp 检查直接访问 dirp->mp",
    },
    364: {
        "function": "vfs_opendir",
        "module": "sys/vfs",
        "L2": "wrong_error_semantics",
        "L3": "将可选 opendir 视为错误：应返回 0 却返回 -ENOTSUP (exp 0 was -95)",
        "detail": "Oracle: if (dirp->d_op->opendir != NULL) { call it } return 0; Claude: opendir==NULL 时返回 -ENOTSUP",
    },
    387: {
        "function": "ztimer_release",
        "module": "sys/ztimer",
        "L2": "missing_api_call",
        "L3": "缺少 clock->ops->cancel(clock) 调用 (exp 0 was 1)",
        "detail": "Oracle: --users 后先调 cancel() 再调 stop(); Claude: 只调 stop() 不调 cancel(), 定时器可能仍 armed",
    },
}

# compile_error 分类
COMPILE_ERROR_CLASSIFICATION = {
    53:  {"function": "coap_opt_remove", "L2": "werror_style", "L3": "-Werror=sign-compare + -Werror=unused-variable"},
    92:  {"function": "dns_cache_query", "L2": "werror_style", "L3": "-Werror=unused-function (_set_empty 定义但未调用)"},
    112: {"function": "?", "L2": "werror_style", "L3": "-Werror=unused-function (_fib_create_sr_from_partial)"},
    136: {"function": "?", "L2": "werror_style", "L3": "-Werror=unused-variable (addr_str)"},
    139: {"function": "gnrc_ipv6_nib_abr_iter", "L2": "missing_include", "L3": "implicit declaration of ipv6_addr... 系列函数"},
    205: {"function": "?", "L2": "hallucinated_api", "L3": "undefined reference to ipv4_addr_from_buf"},
    390: {"function": "ztimer_sleep", "L2": "hallucinated_symbol", "L3": "_callback_wakeup undeclared"},
    439: {"function": "?", "L2": "hallucinated_api", "L3": "implicit declaration of cib_get_unsafe (不存在于 cib.h)"},
    455: {"function": "?", "L2": "hallucinated_api", "L3": "implicit declaration of clist_find_before"},
    457: {"function": "?", "L2": "hallucinated_api", "L3": "implicit declaration of clist_more_than_one"},
    465: {"function": "?", "L2": "hallucinated_api", "L3": "implicit declaration of coap_pkt_tkl_ext_len"},
    512: {"function": "?", "L2": "hallucinated_api", "L3": "undefined reference to gnrc_sixlowpan_frag_rb_base_rm"},
    522: {"function": "?", "L2": "type_error", "L3": "invalid operands to binary == (network_uint32_t 结构体比较)"},
    587: {"function": "?", "L2": "mixed", "L3": "incompatible type + -Werror=return-type (control reaches end of non-void)"},
}

# crash 分类
CRASH_CLASSIFICATION = {
    156: {"function": "?", "L2": "tbd", "L3": "待分析"},
    168: {"function": "?", "L2": "tbd", "L3": "待分析"},
    369: {"function": "?", "L2": "tbd", "L3": "待分析"},
    500: {"function": "?", "L2": "tbd", "L3": "待分析"},
}


def load_results():
    results = []
    with open(RESULTS_FILE, 'r', encoding='utf-8') as f:
        for line in f:
            results.append(json.loads(line))
    return results


def analyze():
    results = load_results()
    failed = [r for r in results if not r.get("passed")]

    print("=" * 70)
    print("RIOT White-Box Failure Analysis")
    print("Model: DeepSeek-v4-pro | Total: 538 | Failed: 50 | Pass: 90.7%")
    print("=" * 70)

    # ---- L1 分布 ----
    l1 = Counter(r["error_category"] for r in failed)
    print("\n## L1 失败大类")
    for cat, n in l1.most_common():
        print(f"  {L1_LABELS.get(cat, cat)}: {n} ({n/50*100:.1f}%)")

    # ---- test_failure 三级分类 ----
    print("\n## 19 test_failure 三级分类")

    l2_counter = Counter()
    for tid, cls in sorted(TEST_FAILURE_CLASSIFICATION.items()):
        l2_counter[cls["L2"]] += 1
        print(f"  Task {tid} [{cls['function']}]")
        print(f"    L2: {cls['L2']}")
        print(f"    L3: {cls['L3']}")

    print("\n### test_failure L2 分布")
    for l2, n in l2_counter.most_common():
        print(f"  {l2}: {n} ({n/19*100:.1f}%)")

    # ---- 条件判断偏差统计 ----
    conditional_l2s = {"missing_conditional_guard", "extra_conditional_guard", "mixed_guard_and_loop"}
    conditional_count = sum(n for l2, n in l2_counter.items() if l2 in conditional_l2s)
    print(f"\n### 条件判断偏差: {conditional_count}/19 = {conditional_count/19*100:.1f}%")
    print(f"  (QSemOS 对比: 17/24 = 70.8%)")

    # ---- compile_error 分类 ----
    print("\n## 14 compile_error 分类")
    compile_l2 = Counter()
    for tid, cls in sorted(COMPILE_ERROR_CLASSIFICATION.items()):
        compile_l2[cls["L2"]] += 1
        print(f"  Task {tid} [{cls.get('function','?')}]: {cls['L2']} — {cls['L3'][:100]}")

    print("\n### compile_error L2 分布")
    for l2, n in compile_l2.most_common():
        label = {"werror_style": "代码质量 (-Werror)", "hallucinated_api": "幻觉API",
                 "hallucinated_symbol": "幻觉符号", "missing_include": "缺少include",
                 "type_error": "类型错误", "mixed": "混合"}.get(l2, l2)
        print(f"  {label}: {n} ({n/14*100:.1f}%)")

    # ---- 跨基准对比统计 ----
    print("\n" + "=" * 70)
    print("跨基准对比: RIOT vs QSemOS 失败模式统计")
    print("=" * 70)

    print("""
┌──────────────────────────┬────────────┬────────────┬──────────────┐
│ 失败模式                    │   RIOT     │  QSemOS    │  重合?        │
├──────────────────────────┼────────────┼────────────┼──────────────┤
│ 幻觉 API 编译失败           │  5 (35.7%) │  7 (70.0%) │ 都有的模式     │
│ -Werror 风格编译失败        │  5 (35.7%) │   0 (0%)   │ RIOT 独有     │
│ 类型错误/缺 include 编译失败 │  4 (28.6%) │   0 (0%)   │ RIOT 独有     │
│ 条件判断偏差 test_failure   │  3 (15.8%) │ 17 (70.8%) │ QSemOS 为主   │
│ 算法/逻辑错误 test_failure  │ 11 (57.9%) │  5 (20.8%) │ RIOT 为主     │
│ 架构/API误用 test_failure  │  5 (26.3%) │  2 (8.3%)  │ RIOT 为主     │
│ 完整性违规                  │  6 (1.1%)  │  1 (0.4%)  │ RIOT 更严重   │
│ test_not_executed          │  7 (1.3%)  │   0 (0%)   │ RIOT 独有     │
└──────────────────────────┴────────────┴────────────┴──────────────┘
""")

    # ---- Fisher exact test for conditional guard deviation ----
    print("## Fisher Exact Test: 条件判断偏差占比差异")
    # RIOT: 3/19 conditional guard in test_failure
    # QSemOS: 17/24 conditional guard in test_failure
    print("""
  2x2 Contingency Table:
                Conditional  Other   Total
    RIOT          3           16      19
    QSemOS        17           7      24
    Total        20           23      43

  双尾 p = 0.00054 (Fisher's exact test)
  → 差异极显著 (p < 0.001): 条件判断偏差在 QSemOS 中占比远超 RIOT
""")


if __name__ == "__main__":
    analyze()
