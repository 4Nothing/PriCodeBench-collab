#include <stdio.h>
#include <string.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"

int main(void)
{
    Test_InitWheel();
    Test_ResetGlobals();
    struct TestStats st;
    Test_InitStats(&st);

    struct TagSwTmrCtrl t = {0};
    t.coreID = 0;

    // Case 1: basic lock returns a token (stub returns 0)
    uintptr_t token = OsSwtmrIqrSplLock(&t);
    Test_CheckEqU32("lock returns 0", 0, (U32)token, &st);

    // Case 2: repeated lock still returns 0 in stub
    token = OsSwtmrIqrSplLock(&t);
    Test_CheckEqU32("relock returns 0", 0, (U32)token, &st);

    // Case 3: different coreID values do not affect stub
    t.coreID = 1;
    token = OsSwtmrIqrSplLock(&t);
    Test_CheckEqU32("lock core 1 -> 0", 0, (U32)token, &st);

    // Case 4: timer fields remain unchanged by lock
    t.interval = 123;
    token = OsSwtmrIqrSplLock(&t);
    Test_CheckEqU32("interval unchanged", 123, t.interval, &st);

    // Case 5: null-like behavior is undefined; pass a dummy struct on stack
    struct TagSwTmrCtrl dummy;
    memset(&dummy, 0, sizeof(dummy));
    token = OsSwtmrIqrSplLock(&dummy);
    Test_CheckEqU32("dummy lock ok", 0, (U32)token, &st);

    Test_Summary("OsSwtmrIqrSplLock", &st);
    return (st.pass == st.total) ? 0 : 1;
}
