#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"

U32 OsHwiSetAttrParaCheck(HwiHandle hwiNum, HwiPrior prio, HwiMode mode);
U32 OsHwiSetAttrConflictErrCheck(U32 irqNum, HwiPrior prio, HwiMode mode);
void OsHwiDescInitAll(void);

#include "func_under_test.c"

int main(void)
{
    int passed = 0, total = 0;
    OsHwiDescInitAll();

    /* 1. Invalid hwi number */
    U32 ret = PRT_HwiSetAttr(OS_HWI_MAX_NUM, 0, 0);
    print_case_result("invalid hwi num", OS_ERRNO_HWI_NUM_INVALID, ret, ret == OS_ERRNO_HWI_NUM_INVALID);
    passed += (ret == OS_ERRNO_HWI_NUM_INVALID);
    total++;

    /* 2. Invalid priority */
    ret = PRT_HwiSetAttr(0, 300u, 0);
    print_case_result("invalid priority", OS_ERRNO_HWI_PRI_ERROR, ret, ret == OS_ERRNO_HWI_PRI_ERROR);
    passed += (ret == OS_ERRNO_HWI_PRI_ERROR);
    total++;

    /* 3. Invalid mode */
    ret = PRT_HwiSetAttr(0, 10u, 5u);
    print_case_result("invalid mode", OS_ERRNO_HWI_MODE_ERROR, ret, ret == OS_ERRNO_HWI_MODE_ERROR);
    passed += (ret == OS_ERRNO_HWI_MODE_ERROR);
    total++;

    /* 4. Valid set */
    ret = PRT_HwiSetAttr(17, 7u, 1u);
    long long exp = OS_OK;
    long long act = ret;
    print_case_result("set attr ok", exp, act, act == exp);
    passed += (act == exp);
    total++;
    long long eprio = 7;
    long long aprio = OS_HWI_MODE_ATTR(1)->prior;
    print_case_result("check prior", eprio, aprio, eprio == aprio);
    passed += (eprio == aprio);
    total++;
    long long emode = 1;
    long long amode = OS_HWI_MODE_ATTR(1)->mode;
    print_case_result("check mode", emode, amode, emode == amode);
    passed += (emode == amode);
    total++;

    /* 5. Another valid set */
    ret = PRT_HwiSetAttr(OS_HWI_MAX_NUM - 1, 0u, 0u);
    print_case_result("set attr edge", OS_OK, ret, ret == OS_OK);
    passed += (ret == OS_OK);
    total++;

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}
