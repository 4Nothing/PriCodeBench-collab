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

    // Case 1: unlock with 0 token should not crash and does not change fields (stub)
    OsSwtmrIqrSplUnlock(&t, 0);
    Test_CheckEqU32("unlock ok", 0, 0, &st);

    // Case 2: unlock with arbitrary token
    OsSwtmrIqrSplUnlock(&t, 1234);
    Test_CheckEqU32("unlock token ignored", 0, 0, &st);

    // Case 3: unlock after lock
    uintptr_t tok = OsSwtmrIqrSplLock(&t);
    OsSwtmrIqrSplUnlock(&t, tok);
    Test_CheckEqU32("unlock after lock", 0, 0, &st);

    // Case 4: multiple unlocks harmless in stub
    OsSwtmrIqrSplUnlock(&t, tok);
    Test_CheckEqU32("double unlock", 0, 0, &st);

    // Case 5: unlocking different timer also ok
    struct TagSwTmrCtrl t2 = {0};
    OsSwtmrIqrSplUnlock(&t2, tok);
    Test_CheckEqU32("unlock other timer", 0, 0, &st);

    Test_Summary("OsSwtmrIqrSplUnlock", &st);
    return (st.pass == st.total) ? 0 : 1;
}
