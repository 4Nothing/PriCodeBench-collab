OS_SEC_L4_TEXT void OsTskStackInit(U32 stackSize, uintptr_t topStack)
{
    U32 loop;
    U32 stackMagicWord = OS_TSK_STACK_MAGIC;

    /* 初始化任务栈，并写入栈魔术字 */
    for (loop = 1; loop < (stackSize / sizeof(U32)); loop++) {
        *((U32 *)topStack + loop) = stackMagicWord;
    }
    *((U32 *)(topStack)) = OS_TSK_STACK_TOP_MAGIC;
}
