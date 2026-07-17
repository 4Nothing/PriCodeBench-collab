/*
 * Copyright (c) 2020 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include "coredump_internal.h"

extern uintptr_t __data_start;
extern uintptr_t __data_end;
extern uintptr_t __bss_end__;
extern uintptr_t __bss_start__;
//填入需保存的内存区域,比如如下的数据段与bss段
struct z_coredump_memory_region_t z_coredump_memory_regions[] = {
	// {(uintptr_t)&__data_start, (uintptr_t)&__data_end},
	// {(uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__},
	{0, 0} /* End of list */
};
