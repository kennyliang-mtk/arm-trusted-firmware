/*
 * Copyright (c) 2015-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <sspm_reg.h>
#include <mtk_mcdi.h>

static inline uint32_t mcdi_mbox_read(uint32_t id)
{
	return mmio_read_32(SSPM_MBOX_3_BASE + (id << 2));
}

static inline void mcdi_mbox_write(uint32_t id, uint32_t val)
{
	mmio_write_32(SSPM_MBOX_3_BASE + (id << 2), val);
}

void sspm_set_bootaddr(uint32_t bootaddr)
{
	mcdi_mbox_write(MCDI_MBOX_BOOTADDR, bootaddr);
}

void sspm_cluster_pwr_off_notify(uint32_t cluster)
{
	mcdi_mbox_write(MCDI_MBOX_CLUSTER_0_ATF_ACTION_DONE + cluster, 1);
}

void sspm_cluster_pwr_on_notify(uint32_t cluster)
{
	mcdi_mbox_write(MCDI_MBOX_CLUSTER_0_ATF_ACTION_DONE + cluster, 0);
}

void sspm_standbywfi_irq_enable(uint32_t cpu)
{
	mmio_write_32(SSPM_CFGREG_ACAO_INT_SET, STANDBYWFI_EN(cpu));
}

uint32_t mcdi_avail_cpu_mask_read(void)
{
	return mcdi_mbox_read(MCDI_MBOX_AVAIL_CPU_MASK);
}

uint32_t mcdi_avail_cpu_mask_write(uint32_t mask)
{
	mcdi_mbox_write(MCDI_MBOX_AVAIL_CPU_MASK, mask);

	return mask;
}

uint32_t mcdi_avail_cpu_mask_set(uint32_t mask)
{
	uint32_t m;

	m = mcdi_mbox_read(MCDI_MBOX_AVAIL_CPU_MASK);
	m |= mask;
	mcdi_mbox_write(MCDI_MBOX_AVAIL_CPU_MASK, m);

	return m;
}

uint32_t mcdi_avail_cpu_mask_clr(uint32_t mask)
{
	uint32_t m;

	m = mcdi_mbox_read(MCDI_MBOX_AVAIL_CPU_MASK);
	m &= ~mask;
	mcdi_mbox_write(MCDI_MBOX_AVAIL_CPU_MASK, m);

	return m;
}

uint32_t mcdi_cpu_cluster_pwr_stat_read(void)
{
	return mcdi_mbox_read(MCDI_MBOX_CPU_CLUSTER_PWR_STAT);
}

void mcdi_init(void)
{
	mcdi_avail_cpu_mask_write(0x01); /* cpu0 default on */
}
