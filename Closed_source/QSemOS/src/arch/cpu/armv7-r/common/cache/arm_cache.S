/*
 * Copyright (c) 2008-2012 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <prt_buildef.h>
#include "prt_asm_arm_external.h"

.text

/* void arch_disable_cache(uint flags) */
FUNCTION(arch_disable_cache)
    stmfd   sp!, {r4-r11, lr}

    mov     r7, r0                      // save flags

    mrs     r8, cpsr                    // save the old interrupt state
    cpsid   iaf                         // interrupts disabled

.Ldcache_disable:
    tst     r7, #DCACHE
    beq     .Licache_disable
    mrc     p15, 0, r0, c1, c0, 0       // cr1
    tst     r0, #(1<<2)                 // is the dcache already disabled?
    beq     .Ldcache_already_disabled

    bic     r0, #(1<<2)
    mcr     p15, 0, r0, c1, c0, 0       // disable dcache

    // flush and invalidate the dcache
    // NOTE: trashes a bunch of registers, can't be spilling stuff to the stack
    bl      flush_invalidate_cache_v7

    b       .Ldcache_disable_L2

.Ldcache_already_disabled:
    // make sure all of the caches are invalidated
    // NOTE: trashes a bunch of registers, can't be spilling stuff to the stack
    bl      invalidate_cache_v7

.Ldcache_disable_L2:

#ifdef CONFIG_ARCH_L2CACHE
    // disable the L2, if present
    mrc     p15, 0, r0, c1, c0, 1       // aux cr1
    bic     r0, #(1<<1)
    mcr     p15, 0, r0, c1, c0, 1       // disable L2 dcache
#endif

.Licache_disable:
    tst     r7, #ICACHE
    beq     .Ldone_disable

    mrc     p15, 0, r0, c1, c0, 0       // cr1
    bic     r0, #(1<<12)
    mcr     p15, 0, r0, c1, c0, 0       // disable icache

.Ldone_disable:
    // make sure the icache is always invalidated
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       // invalidate icache to PoU

    msr     cpsr, r8
    ldmfd   sp!, {r4-r11, pc}

/* void arch_enable_cache(uint flags) */
FUNCTION(arch_enable_cache)
    stmfd   sp!, {r4-r12, lr}

    mov     r7, r0                      // save flags

    mrs     r8, cpsr                    // save the old interrupt state
    cpsid   iaf                         // interrupts disabled

.Ldcache_enable:
    tst     r7, #DCACHE
    beq     .Licache_enable
    mrc     p15, 0, r0, c1, c0, 0       // cr1
    tst     r0, #(1<<2)                 // is the dcache already enabled?
    bne     .Licache_enable

    // invalidate L1 and L2
    // NOTE: trashes a bunch of registers, can't be spilling stuff to the stack
    bl      invalidate_cache_v7

#ifdef CONFIG_ARCH_L2CACHE
    // enable the L2, if present
    mrc     p15, 0, r0, c1, c0, 1       // aux cr1
    orr     r0, #(1<<1)
    mcr     p15, 0, r0, c1, c0, 1       // enable L2 dcache
#endif

    mrc     p15, 0, r0, c1, c0, 0       // cr1
    orr     r0, #(1<<2)
    mcr     p15, 0, r0, c1, c0, 0       // enable dcache

.Licache_enable:
    tst     r7, #ICACHE
    beq     .Ldone_enable

    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       // invalidate icache to PoU

    mrc     p15, 0, r0, c1, c0, 0       // cr1
    orr     r0, #(1<<12)
    mcr     p15, 0, r0, c1, c0, 0       // enable icache

.Ldone_enable:
    isb
    msr     cpsr, r8
    ldmfd   sp!, {r4-r12, pc}

// flush & invalidate cache routine, trashes r0-r6, r9-r11
flush_invalidate_cache_v7:
    /* from ARMv7 manual, B2-17 */
    dmb
    MRC     p15, 1, R0, c0, c0, 1       // Read CLIDR
    ANDS    R3, R0, #0x7000000
    MOV     R3, R3, LSR #23             // Cache level value (naturally aligned)
    BEQ     .Lfinished
    MOV     R10, #0
.Loop1:
    ADD     R2, R10, R10, LSR #1        // Work out 3xcachelevel
    MOV     R1, R0, LSR R2              // bottom 3 bits are the Cache type for this level
    AND     R1, R1, #7                  // get those 3 bits alone
    CMP     R1, #2
    BLT     .Lskip                      // no cache or only instruction cache at this level
    MCR     p15, 2, R10, c0, c0, 0      // write the Cache Size selection register
    isb                                 // ISB to sync the change to the CacheSizeID reg
    MRC     p15, 1, R1, c0, c0, 0       // reads current Cache Size ID register
    AND     R2, R1, #0x7                // extract the line length field
    ADD     R2, R2, #4                  // add 4 for the line length offset (log2 16 bytes)
    LDR     R4, =0x3FF
    ANDS    R4, R4, R1, LSR #3          // R4 is the max number on the way size (right aligned)
    CLZ     R5, R4                      // R5 is the bit position of the way size increment
    LDR     R6, =0x00007FFF
    ANDS    R6, R6, R1, LSR #13         // R6 is the max number of the index size (right aligned)
.Loop2:
    MOV     R9, R4                      // R9 working copy of the max way size (right aligned)
.Loop3:
    ORR     R11, R10, R9, LSL R5        // factor in the way number and cache number into R11
    ORR     R11, R11, R6, LSL R2        // factor in the index number
    MCR     p15, 0, R11, c7, c14, 2     // clean & invalidate by set/way
    SUBS    R9, R9, #1                  // decrement the way number
    BGE     .Loop3
    SUBS    R6, R6, #1                  // decrement the index
    BGE     .Loop2
.Lskip:
    ADD     R10, R10, #2                // increment the cache number
    CMP     R3, R10
    BGT     .Loop1

.Lfinished:
    mov     r10, #0
    mcr     p15, 2, r10, c0, c0, 0      // select cache level 0
    dsb
    isb

    bx      lr

// invalidate cache routine, trashes r0-r6, r9-r11
invalidate_cache_v7:
    /* from ARMv7 manual, B2-17 */
    dmb
    MRC     p15, 1, R0, c0, c0, 1       // Read CLIDR
    ANDS    R3, R0, #0x7000000
    MOV     R3, R3, LSR #23             // Cache level value (naturally aligned)
    BEQ     .Lfinished_invalidate
    MOV     R10, #0
.Loop1_invalidate:
    ADD     R2, R10, R10, LSR #1        // Work out 3xcachelevel
    MOV     R1, R0, LSR R2              // bottom 3 bits are the Cache type for this level
    AND     R1, R1, #7                  // get those 3 bits alone
    CMP     R1, #2
    BLT     .Lskip_invalidate           // no cache or only instruction cache at this level
    MCR     p15, 2, R10, c0, c0, 0      // write the Cache Size selection register
    isb                                 // ISB to sync the change to the CacheSizeID reg
    MRC     p15, 1, R1, c0, c0, 0       // reads current Cache Size ID register
    AND     R2, R1, #0x7                // extract the line length field
    ADD     R2, R2, #4                  // add 4 for the line length offset (log2 16 bytes)
    LDR     R4, =0x3FF
    ANDS    R4, R4, R1, LSR #3          // R4 is the max number on the way size (right aligned)
    CLZ     R5, R4                      // R5 is the bit position of the way size increment
    LDR     R6, =0x00007FFF
    ANDS    R6, R6, R1, LSR #13         // R6 is the max number of the index size (right aligned)
.Loop2_invalidate:
    MOV     R9, R4                      // R9 working copy of the max way size (right aligned)
.Loop3_invalidate:
    ORR     R11, R10, R9, LSL R5        // factor in the way number and cache number into R11
    ORR     R11, R11, R6, LSL R2        // factor in the index number
    MCR     p15, 0, R11, c7, c6, 2      // invalidate by set/way
    SUBS    R9, R9, #1                  // decrement the way number
    BGE     .Loop3_invalidate
    SUBS    R6, R6, #1                  // decrement the index
    BGE     .Loop2_invalidate
.Lskip_invalidate:
    ADD     R10, R10, #2                // increment the cache number
    CMP     R3, R10
    BGT     .Loop1_invalidate

.Lfinished_invalidate:
    dsb
    mov     r10, #0
    mcr     p15, 2, r10, c0, c0, 0      // select cache level 0
    isb

    bx      lr

    /* void arch_flush_cache_range(addr_t start, size_t len); */
FUNCTION(arch_clean_cache_range)
#ifdef CONFIG_ARM_WITH_CP15
    mov     r3, r0                      // save the start address
    add     r2, r0, r1                  // calculate the end address
    bic     r0, #(OS_CACHE_LINE_SIZE-1)     // align the start with a cache line
0:
    mcr     p15, 0, r0, c7, c10, 1      // clean cache to PoC by MVA
    add     r0, #OS_CACHE_LINE_SIZE
    cmp     r0, r2
    blo     0b

    dsb
#endif
    bx      lr

    /* void arch_flush_invalidate_cache_range(addr_t start, size_t len); */
FUNCTION(arch_clean_invalidate_cache_range)
#ifdef CONFIG_ARM_WITH_CP15
    mov     r3, r0                      // save the start address
    add     r2, r0, r1                  // calculate the end address
    bic     r0, #(OS_CACHE_LINE_SIZE-1)     // align the start with a cache line
0:
    mcr     p15, 0, r0, c7, c14, 1      // clean & invalidate dcache to PoC by MVA
    add     r0, r0, #OS_CACHE_LINE_SIZE
    cmp     r0, r2
    blo     0b

    dsb
#endif
    bx      lr

    /* void arch_clean_invalidate_dcache_all(void); */
FUNCTION(arch_clean_invalidate_dcache_all)
#ifdef CONFIG_ARM_WITH_CP15
	mrc		p15, 1, r0, c0, c0, 0   /* Read the Cache Size Identification Register */
	ldr		r3, =0xffff			/* Isolate the NumSets field (bits 13-27) */
	and		r0, r3, r0, lsr #13		/* r0=NumSets (number of sets - 1) */

	mov		r1, #0				/* r1 = way loop counter */
way_loop:

	mov		r3, #0				/* r3 = set loop counter */
set_loop:
	mov		r2, r1, lsl #30			/* r2 = way loop counter << 30 */
	orr		r2, r3, lsl #5			/* r2 = set/way cache operation format */
	mcr		p15, 0, r2, c7, c14, 2  /* Data Cache Clean Invalidate by Set/Way */
	add		r3, r3, #1			/* Increment set counter */
	cmp		r0, r3				/* Last set? */
	bne		set_loop			/* Keep looping if not */

	add		r1, r1, #1			/* Increment the way counter */
	cmp		r1, #4				/* Last way? (four ways assumed) */
	bne		way_loop			/* Keep looping if not */

	dsb
#endif
    bx      lr

    /* void arch_invalidate_cache_range(addr_t start, size_t len); */
FUNCTION(arch_invalidate_cache_range)
#ifdef CONFIG_ARM_WITH_CP15
    mov     r3, r0                      // save the start address
    add     r2, r0, r1                  // calculate the end address
    bic     r0, #(OS_CACHE_LINE_SIZE-1)     // align the start with a cache line
0:
    mcr     p15, 0, r0, c7, c6, 1       // invalidate dcache to PoC by MVA
    add     r0, r0, #OS_CACHE_LINE_SIZE
    cmp     r0, r2
    blo     0b

    dsb
#endif
    bx      lr

    /* void arch_sync_cache_range(addr_t start, size_t len); */
FUNCTION(arch_sync_cache_range)
    push    { r14 }
    bl      arch_clean_cache_range

    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       // invalidate icache to PoU

    pop     { pc }
