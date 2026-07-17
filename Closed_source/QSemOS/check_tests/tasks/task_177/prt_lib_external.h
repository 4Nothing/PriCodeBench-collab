#ifndef MOCK_PRT_LIB_EXTERNAL_H
#define MOCK_PRT_LIB_EXTERNAL_H

/* Placeholder for OS_GOTO_SYS_ERROR1 and others used in production */
/* In tests, we just return error codes directly so this macro can be no-op or marker. */
#define OS_GOTO_SYS_ERROR1() \
    do                       \
    {                        \
    } while (0)

#endif /* MOCK_PRT_LIB_EXTERNAL_H */
