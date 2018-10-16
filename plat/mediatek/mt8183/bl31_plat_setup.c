/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <bl_common.h>
#include <common_def.h>
#include <console.h>
#include <debug.h>
#include <generic_delay_timer.h>
#include <mcucfg.h>
#include <mmio.h>
#include <plat_arm.h>
#include <plat_private.h>
#include <platform_def.h>

static entry_point_info_t bl32_ep_info;
static entry_point_info_t bl33_ep_info;

static void platform_setup_cpu(void)
{
	mmio_write_32((uintptr_t)&mt8183_mcucfg->mp0_rw_rsvd0, 0x00000001);

#if PLATFORM_DEBUG
	NOTICE("addr of cci_adb400_dcm_config: 0x%x\n",
		mmio_read_32((uintptr_t)&mt8183_mcucfg->cci_adb400_dcm_config));
	NOTICE("addr of sync_dcm_config: 0x%x\n",
		mmio_read_32((uintptr_t)&mt8183_mcucfg->sync_dcm_config));

	NOTICE("mp0_spmc: 0x%x\n", mmio_read_32((uintptr_t)&mt8183_mcucfg->mp0_cputop_spmc_ctl));
	NOTICE("mp1_spmc: 0x%x\n", mmio_read_32((uintptr_t)&mt8183_mcucfg->mp1_cputop_spmc_ctl));
#endif
}

/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info;

	next_image_info = (type == NON_SECURE) ? &bl33_ep_info : &bl32_ep_info;

	/* None of the images on this platform can have 0x0 as the entrypoint */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

/*******************************************************************************
 * Perform any BL3-1 early platform setup. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables.
 * BL2 has flushed this information to memory, so we are guaranteed to pick up
 * good data.
 ******************************************************************************/
void bl31_early_platform_setup(bl31_params_t *from_bl2,
			       void *plat_params_from_bl2)
{
	console_init(UART0_BASE, UART_CLOCK, UART_BAUDRATE);

	NOTICE("MT8183 bl31_setup\n");

	assert(from_bl2 != NULL);
	assert(from_bl2->h.type == PARAM_BL31);
	assert(from_bl2->h.version >= VERSION_1);

	bl32_ep_info = *from_bl2->bl32_ep_info;
	bl33_ep_info = *from_bl2->bl33_ep_info;
}

/*******************************************************************************
 * Perform any BL3-1 platform setup code
 ******************************************************************************/
void bl31_platform_setup(void)
{
	platform_setup_cpu();

	generic_delay_timer_init();

	/* Initialize the gic cpu and distributor interfaces */
	plat_gic_init();

#if ENABLE_PLAT_COMPAT
	/* Topologies are best known to the platform. */
	mt_setup_topology();
#endif
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
	plat_cci_init();
	plat_cci_enable();

	plat_configure_mmu_el3(BL_CODE_BASE,
			       BL_COHERENT_RAM_END - BL_CODE_BASE,
			       BL_CODE_BASE,
			       BL_CODE_END,
			       BL_COHERENT_RAM_BASE,
			       BL_COHERENT_RAM_END);
}
