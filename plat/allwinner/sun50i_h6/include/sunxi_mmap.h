/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SUNXI_MMAP_H
#define SUNXI_MMAP_H

/* Memory regions */
#define SUNXI_ROM_BASE			0x00000000
#define SUNXI_ROM_SIZE			0x00010000
#define SUNXI_SRAM_BASE			0x00020000
#define SUNXI_SRAM_SIZE			0x000f8000
#define SUNXI_SRAM_A1_BASE		0x00020000
#define SUNXI_SRAM_A1_SIZE		0x00008000
#define SUNXI_SRAM_A2_BASE		0x00104000
#define SUNXI_SRAM_A2_SIZE		0x00014000
#define SUNXI_SRAM_C_BASE		0x00028000
#define SUNXI_SRAM_C_SIZE		0x0001e000
#define SUNXI_DEV_BASE			0x01000000
#define SUNXI_DEV_SIZE			0x09000000
#define SUNXI_DRAM_BASE			0x40000000
#define SUNXI_DRAM_VIRT_BASE		0x0a000000

/* Memory-mapped devices */
#define SUNXI_SYSCON_BASE		0x03000000
#define SUNXI_CPUCFG_BASE		0x09010000
#define SUNXI_SID_BASE			0x03006000
#define SUNXI_DMA_BASE			0x03002000
#define SUNXI_MSGBOX_BASE		0x03003000
#define SUNXI_CCU_BASE			0x03010000
#define SUNXI_CCU_SEC_SWITCH_REG	(SUNXI_CCU_BASE + 0xf00)
#define SUNXI_PIO_BASE			0x030b0000
#define SUNXI_TIMER_BASE		0x03009000
#define SUNXI_WDOG_BASE			0x030090a0
#define SUNXI_THS_BASE			0x05070400
#define SUNXI_UART0_BASE		0x05000000
#define SUNXI_UART1_BASE		0x05000400
#define SUNXI_UART2_BASE		0x05000800
#define SUNXI_UART3_BASE		0x05000c00
#define SUNXI_I2C0_BASE			0x05002000
#define SUNXI_I2C1_BASE			0x05002400
#define SUNXI_I2C2_BASE			0x05002800
#define SUNXI_I2C3_BASE			0x05002c00
#define SUNXI_SPI0_BASE			0x05010000
#define SUNXI_SPI1_BASE			0x05011000
#define SUNXI_SCU_BASE			0x03020000
#define SUNXI_GICD_BASE			0x03021000
#define SUNXI_GICC_BASE			0x03022000
#define SUNXI_R_TIMER_BASE		0x07020000
#define SUNXI_R_INTC_BASE		0x07021000
#define SUNXI_R_WDOG_BASE		0x07020400
#define SUNXI_R_PRCM_BASE		0x07010000
#define SUNXI_R_TWD_BASE		0x07020800
#define SUNXI_R_CPUCFG_BASE		0x07000400
#define SUNXI_R_I2C_BASE		0x07081400
#define SUNXI_R_UART_BASE		0x07080000
#define SUNXI_R_PIO_BASE		0x07022000

#endif /* SUNXI_MMAP_H */
