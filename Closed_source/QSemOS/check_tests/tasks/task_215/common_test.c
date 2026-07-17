#include "common_test.h"

volatile U32 OS_INT_ACTIVE = 0;
volatile U32 OS_TASK_LOCK_DATA = 0;
struct TagOsRunQue g_rq = {0, 0, 0};
struct TagTskCb *g_running_task = NULL;

int g_schedule_fast_called = 0;
int g_schedule_fast_ps_called = 0;
int g_smp_schedule_trigger_called = 0;
int g_spin_lock_called = 0;
int g_spin_unlock_called = 0;
int g_ready_del_called = 0;
int g_timer_add_called = 0;

U32 g_next_intsave_value = 100;
int g_int_lock_called = 0;
int g_int_restore_called = 0;
U32 g_last_intsave_returned = 0;
U32 g_last_intrestore_arg = 0;

int g_di_state_check_return = 0;

struct TagListObject g_ready_queues[PRIORITY_MAX];

U32 g_last_error_code = 0;

#define REG_MAX 64
static struct TagTskCb *g_registry[REG_MAX];

void ListInit(struct TagListObject *head) { head->prev = head->next = head; }
void ListAdd(struct TagListObject *node, struct TagListObject *head)
{
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}
void ListTailAdd(struct TagListObject *node, struct TagListObject *head)
{
    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
}
void ListDelete(struct TagListObject *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = NULL;
}

void RegisterTcb(struct TagTskCb *tcb)
{
    if (tcb->taskPid < REG_MAX)
        g_registry[tcb->taskPid] = tcb;
}
struct TagTskCb *GET_TCB_HANDLE(TskHandle id) { return (id < REG_MAX) ? g_registry[id] : NULL; }

void test_reset_state(void)
{
    OS_INT_ACTIVE = 0;
    OS_TASK_LOCK_DATA = 0;
    g_rq.uniTaskLock = 0;
    g_rq.needReschedule = 0;
    g_rq.uniFlag = 0;
    g_running_task = NULL;
    g_schedule_fast_called = 0;
    g_schedule_fast_ps_called = 0;
    g_smp_schedule_trigger_called = 0;
    g_spin_lock_called = 0;
    g_spin_unlock_called = 0;
    g_ready_del_called = 0;
    g_timer_add_called = 0;
    g_next_intsave_value = 100;
    g_int_lock_called = 0;
    g_int_restore_called = 0;
    g_last_intsave_returned = 0;
    g_last_intrestore_arg = 0;
    g_di_state_check_return = 0;
    g_last_error_code = 0;
    for (int i = 0; i < PRIORITY_MAX; ++i)
        ListInit(&g_ready_queues[i]);
    for (int i = 0; i < REG_MAX; ++i)
        g_registry[i] = NULL;
    g_total_cases = 0;
    g_pass_cases = 0;
}

void test_reset_env_only(void)
{
    U32 total = g_total_cases, pass = g_pass_cases;
    test_reset_state();
    g_total_cases = total;
    g_pass_cases = pass;
}

int g_total_cases = 0;
int g_pass_cases = 0;

void test_begin(const char *title) { printf("===== %s =====\n", title); }
void test_expect_u32(const char *caseName, U32 expect, U32 actual)
{
    int pass = (expect == actual);
    printf("[%s] expect=%u actual=%u -> %s\n", caseName, expect, actual, pass ? "PASS" : "FAIL");
    g_total_cases++;
    if (pass)
        g_pass_cases++;
}
void test_end(void)
{
    double rate = g_total_cases ? (100.0 * g_pass_cases / g_total_cases) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass_cases, g_total_cases);
}
