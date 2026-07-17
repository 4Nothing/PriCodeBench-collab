#ifndef MOCK_PRT_QUEUE_EXTERNAL_H
#define MOCK_PRT_QUEUE_EXTERNAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* Basic fixed-width types */
    typedef uint8_t U8;
    typedef uint16_t U16;
    typedef uint32_t U32;

/* Visibility and section attributes (no-op for tests) */
#define OS_SEC_BSS
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE static inline

/* Common constants and helpers */
#ifndef NULL
#define NULL ((void *)0)
#endif

#define OS_OK 0U
#define OS_MAX_U16 0xFFFFU

/* Error codes used by the module under test */
#define OS_ERRNO_QUEUE_MAXNUM_ZERO 0x1001U
#define OS_ERRNO_QUEUE_NO_MEMORY 0x1002U
#define OS_ERRNO_QUEUE_CREAT_PTR_NULL 0x1003U
#define OS_ERRNO_QUEUE_PARA_ZERO 0x1004U
#define OS_ERRNO_QUEUE_NSIZE_INVALID 0x1005U
#define OS_ERRNO_QUEUE_CB_UNAVAILABLE 0x1006U
#define OS_ERRNO_QUEUE_CREATE_NO_MEMORY 0x1007U

/* Macros used by the code under test */
#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define OS_QUEUE_NODE_SIZE_ALIGN 4U
#define OS_QUEUE_NODE_HEAD_LEN 4U

/* Queue id and state helpers */
#define OS_QUEUE_ID(idx) ((U32)(idx))
#define OS_QUEUE_UNUSED 0U
#define OS_QUEUE_USED 1U
#define OS_QUEUE_PID_INVALID 0xFFFFFFFFU

    /* Forward declarations of structures used by the C file */
    struct TagQueCb;
    struct QueNode
    {
        U32 srcPid;
    };

    /* Doubly-linked list node placeholder and macros */
    struct ListObject
    {
        struct ListObject *prev;
        struct ListObject *next;
    };
#define INIT_LIST_OBJECT(nodePtr)    \
    do                               \
    {                                \
        (nodePtr)->prev = (nodePtr); \
        (nodePtr)->next = (nodePtr); \
    } while (0)

    struct TagQueCb
    {
        U8 *queue;
        U16 nodeNum;
        U16 nodeSize;
        U32 queueState;
        struct ListObject writeList;
        struct ListObject readList;
        U16 writableCnt;
        U16 readableCnt;
        U16 queueHead;
        U16 queueTail;
        U16 nodePeak;
    };

    /* IRQ lock/unlock stubs used in PRT_QueueCreate */
    static inline void OsIntRestore(uintptr_t x) { (void)x; }
#define QUEUE_INIT_IRQ_LOCK(save) \
    do                            \
    {                             \
        (save) = 0;               \
    } while (0)
#define QUEUE_INIT_IRQ_UNLOCK(save) \
    do                              \
    {                               \
        (void)(save);               \
    } while (0)

    /* Global variables defined in the C file under test (will be linked from it) */
    extern U16 g_maxQueue;
    extern struct TagQueCb *g_allQueue;

#ifdef __cplusplus
}
#endif

#endif /* MOCK_PRT_QUEUE_EXTERNAL_H */
