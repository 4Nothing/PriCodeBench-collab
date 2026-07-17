/*
 * Copyright (c) 2020 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "coredump_internal.h"
#include "prt_sys_external.h"
#include "prt_task_external.h"
#include "prt_irq_external.h"
#include "prt_asm_cpu_external.h"

extern struct coredump_backend_api coredump_backend_print;

static struct coredump_backend_api *backend_api = &coredump_backend_print;


#define INVALIDPID         0xFFFFFFFFUL
/*
* Register block takes up too much stack space
* if defined within function. So define it here.
*/
#ifdef OS_ARCH_ARMV8
/* Identify the version of this block (in case of architecture changes).
* To be interpreted by the target architecture specific block parser.
*/
#define ARCH_HDR_VER			1
static uint64_t arch_blk[22];
#elif defined OS_ARCH_ARMV7_R
/* Identify the version of this block (in case of architecture changes).
* To be interpreted by the target architecture specific block parser.
*/
#define ARCH_HDR_VER			2
static uint32_t arch_blk[17];
#endif

void arch_coredump_info_dump(const struct ExcRegInfo *reg)
{
	/* Target architecture information header */
	/* Information just relevant to the python parser */
	struct coredump_arch_hdr_t hdr = {
		.id = COREDUMP_ARCH_HDR_ID,
		.hdr_version = ARCH_HDR_VER,
		.num_bytes = sizeof(arch_blk),
	};

	(void)memset(&arch_blk, 0, sizeof(arch_blk));

	/*
	* Copies the thread registers to a memory block that will be printed out
	* The thread registers are already provided by structure struct arch_esf
	*/
#ifdef OS_ARCH_ARMV8
	arch_blk[0] = reg->xregs[30];
	arch_blk[1] = reg->xregs[29];
	arch_blk[2] = reg->xregs[28];
	arch_blk[3] = reg->xregs[27];
	arch_blk[4] = reg->xregs[26];
	arch_blk[5] = reg->xregs[25];
	arch_blk[6] = reg->xregs[24];
	arch_blk[7] = reg->xregs[23];
	arch_blk[8] = reg->xregs[22];
	arch_blk[9] = reg->xregs[21];
	arch_blk[10] = reg->xregs[20];
	arch_blk[11] = reg->xregs[19];
	arch_blk[12] = reg->xregs[18];
	arch_blk[13] = reg->xregs[17];
	arch_blk[14] = reg->xregs[16];
	arch_blk[15] = reg->xregs[15];
	arch_blk[16] = reg->xregs[14];
	arch_blk[17] = reg->xregs[13];
	arch_blk[18] = reg->xregs[12];
	arch_blk[19] = reg->elr;
	arch_blk[20] = reg->sp;
	arch_blk[21] = reg->elr;
#elif defined OS_ARCH_ARMV7_R
	arch_blk[0] = reg->R0;
	arch_blk[1] = reg->R1;
	arch_blk[2] = reg->R2;
	arch_blk[3] = reg->R3;
	arch_blk[4] = reg->R12;
	arch_blk[5] = reg->LR;
	arch_blk[6] = reg->PC;
	arch_blk[7] = reg->regCPSR;
	arch_blk[8] = reg->SP;
	arch_blk[9] = reg->R4;
	arch_blk[10] = reg->R5;
	arch_blk[11] = reg->R6;
	arch_blk[12] = reg->R7;
	arch_blk[13] = reg->R8;
	arch_blk[14] = reg->R9;
	arch_blk[15] = reg->R10;
	arch_blk[16] = reg->R11;
#endif
	
	/* Send for output */
	coredump_buffer_output((uint8_t *)&hdr, sizeof(hdr));
	coredump_buffer_output((uint8_t *)arch_blk, sizeof(arch_blk));
}
 

static void dump_header(const struct ExcInfo *info)
{
	struct coredump_hdr_t hdr = {
		.id = {'Z', 'E'},
		.hdr_version = COREDUMP_HDR_VER,
		.reason = info->excCause,
	};

	if (sizeof(uintptr_t) == 8) {
		hdr.ptr_size_bits = 6; /* 2^6 = 64 */
	} else if (sizeof(uintptr_t) == 4) {
		hdr.ptr_size_bits = 5; /* 2^5 = 32 */
	} else {
		hdr.ptr_size_bits = 0; /* Unknown */
	}

	hdr.tgt_code = OS_HARDWARE_PLATFORM;

	backend_api->buffer_output((uint8_t *)&hdr, sizeof(hdr));
}

static void dump_thread(const struct ExcInfo *info)
{
	struct TskInfo taskInfo;

	uintptr_t end_addr;
	uintptr_t start_addr;

	U32 core = THIS_CORE();
	uintptr_t sysStackHigh = OsGetSysStackEnd(core);
    uintptr_t sysStackLow  = OsGetSysStackStart(core);

	if(info->sp <= sysStackHigh && info->sp >= sysStackLow)
	{
		end_addr = sysStackHigh;
		start_addr = sysStackLow;
	} else {
		end_addr = RUNNING_TASK->topOfStack + RUNNING_TASK->stackSize;
		start_addr = RUNNING_TASK->topOfStack;
		PRT_TaskGetInfo(RUNNING_TASK->taskPid, &taskInfo);
	}

	coredump_memory_dump((uintptr_t)(&taskInfo), (uintptr_t)(&taskInfo) + sizeof(taskInfo));

	coredump_memory_dump(start_addr, end_addr);
}

void process_memory_region_list(void)
{
	unsigned int idx = 0;

	while (true) {
		struct z_coredump_memory_region_t *r =
			&z_coredump_memory_regions[idx];

		if (r->end == (uintptr_t)(NULL)) {
			break;
		}

		coredump_memory_dump(r->start, r->end);

		idx++;
	}
}

void coredump(const struct ExcInfo *info)
{
	z_coredump_start();

	dump_header(info);

	arch_coredump_info_dump(&info->regInfo);

	dump_thread(info);

	process_memory_region_list();

	z_coredump_end();
}

void z_coredump_start(void)
{
	backend_api->start();
}

void z_coredump_end(void)
{
	backend_api->end();
}

void coredump_buffer_output(uint8_t *buf, size_t buflen)
{
	if ((buf == NULL) || (buflen == 0)) {
		/* Invalid buffer, skip */
		return;
	}

	backend_api->buffer_output(buf, buflen);
}

void coredump_memory_dump(uintptr_t start_addr, uintptr_t end_addr)
{
	struct coredump_mem_hdr_t m;
	size_t len;

	if ((start_addr == (uintptr_t)(NULL)) ||
	    (end_addr == (uintptr_t)(NULL))) {
		return;
	}

	if (start_addr >= end_addr) {
		return;
	}

	len = end_addr - start_addr;

	m.id = COREDUMP_MEM_HDR_ID;
	m.hdr_version = COREDUMP_MEM_HDR_VER;

	if (sizeof(uintptr_t) == 8) {
		m.start	= (start_addr);
		m.end = (end_addr);
	} else if (sizeof(uintptr_t) == 4) {
		m.start	= (start_addr);
		m.end = (end_addr);
	}

	coredump_buffer_output((uint8_t *)&m, sizeof(m));

	coredump_buffer_output((uint8_t *)start_addr, len);
}

int coredump_query(enum coredump_query_id query_id, void *arg)
{
	int ret;

	if (backend_api->query == NULL) {
		ret = -1;
	} else {
		ret = backend_api->query(query_id, arg);
	}

	return ret;
}

int coredump_cmd(enum coredump_cmd_id cmd_id, void *arg)
{
	int ret;

	if (backend_api->cmd == NULL) {
		ret = -1;
	} else {
		ret = backend_api->cmd(cmd_id, arg);
	}

	return ret;
}
