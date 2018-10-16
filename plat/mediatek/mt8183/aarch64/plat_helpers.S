/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <platform_def.h>

	.globl plat_secondary_cold_boot_setup
	.globl plat_report_exception
	.globl platform_is_primary_cpu
	.globl plat_crash_console_init
	.globl plat_crash_console_putc

	/* -----------------------------------------------------
	 * void plat_secondary_cold_boot_setup (void);
	 *
	 * This function performs any platform specific actions
	 * needed for a secondary cpu after a cold reset e.g
	 * mark the cpu's presence, mechanism to place it in a
	 * holding pen etc.
	 * -----------------------------------------------------
	 */
func plat_secondary_cold_boot_setup
	/* Platform does not do cold boot for secondary CPU */
cb_panic:
	b cb_panic
endfunc plat_secondary_cold_boot_setup

func platform_is_primary_cpu
	and x0, x0, #(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK)
	cmp x0, #PLAT_PRIMARY_CPU
	cset x0, eq
	ret
endfunc platform_is_primary_cpu

	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : x0, x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	mov x0, #0x1
	ret
endfunc plat_crash_console_init

	/* ---------------------------------------------
	 * int plat_crash_console_putc(void)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_putc
	mov_imm x1, UART0_BASE
	b console_core_putc
endfunc plat_crash_console_putc