/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <plat/common/common_def.h>
#include <drivers/console.h>
#include <common/debug.h>
#include <drivers/generic_delay_timer.h>
#include <mcucfg.h>
#include <lib/mmio.h>
#include <mtk_mcdi.h>
#include <mtk_plat_common.h>
#include <mtspmc.h>
#include <params_setup.h>
#include <plat_debug.h>
#include <plat_private.h>
#include <platform_def.h>
#include <scu.h>
#include <spm.h>
#include <drivers/ti/uart/uart_16550.h>

static entry_point_info_t bl32_ep_info;
static entry_point_info_t bl33_ep_info;

static void platform_setup_cpu(void)
{
	mmio_write_32((uintptr_t)&mt8183_mcucfg->mp0_rw_rsvd0, 0x00000001);

	/**
	 * mcusys dcm control
	 */
	/* -enable pll plldiv dcm */
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->bus_pll_divider_cfg,
		BUS_PLLDIV_DCM);
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->mp0_pll_divider_cfg,
		MP0_PLLDIV_DCM);
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->mp2_pll_divider_cfg,
		MP2_PLLDIV_DCM);
	/* - enable mscib dcm  */
	mmio_clrsetbits_32((uintptr_t)&mt8183_mcucfg->mscib_dcm_en,
		MCSIB_CACTIVE_SEL_MASK, MCSIB_CACTIVE_SEL);
	mmio_clrsetbits_32((uintptr_t)&mt8183_mcucfg->mscib_dcm_en,
		MCSIB_DCM_MASK, MCSIB_DCM);
	/* - enable adb400 dcm */
	mmio_clrsetbits_32((uintptr_t)&mt8183_mcucfg->cci_adb400_dcm_config,
		CCI_ADB400_DCM_MASK, CCI_ADB400_DCM);
	/* - enable bus clock dcm */
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->cci_clk_ctrl,
		MCU_BUS_DCM);
	/* - enable bus fabric dcm */
	mmio_clrsetbits_32(
		(uintptr_t)&mt8183_mcucfg->mcusys_bus_fabric_dcm_ctrl,
		MCUSYS_BUS_FABRIC_DCM_MASK,
		MCUSYS_BUS_FABRIC_DCM);
	/* - enable l2c sram dcm */
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->l2c_sram_ctrl,
		L2C_SRAM_DCM);
	/* - enable busmp0 sync dcm */
	mmio_clrsetbits_32((uintptr_t)&mt8183_mcucfg->sync_dcm_config,
		SYNC_DCM_MASK, SYNC_DCM);
	/* - enable cntvalue dcm */
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->mcu_misc_dcm_ctrl,
		CNTVALUEB_DCM);
	/* - enable dcm cluster stall */
	mmio_clrsetbits_32(
		(uintptr_t)&mt8183_mcucfg->sync_dcm_cluster_config,
		MCUSYS_MAX_ACCESS_LATENCY_MASK,
		MCUSYS_MAX_ACCESS_LATENCY);
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->sync_dcm_cluster_config,
		MCU0_SYNC_DCM_STALL_WR_EN);
	/* - enable rgu dcm */
	mmio_setbits_32((uintptr_t)&mt8183_mcucfg->mp0_rgu_dcm_config,
		CPUSYS_RGU_DCM_CINFIG);
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
 * Perform any BL31 early platform setup. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables.
 * BL2 has flushed this information to memory, so we are guaranteed to pick up
 * good data.
 ******************************************************************************/
void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
				u_register_t arg2, u_register_t arg3)
{
	struct mtk_bl31_params *arg_from_bl2 = (struct mtk_bl31_params *)arg0;
	void *plat_params_from_bl2 = (void *) arg1;

	params_early_setup(plat_params_from_bl2);

	static console_16550_t console;
	console_16550_register(UART0_BASE, UART_CLOCK, UART_BAUDRATE, &console);

	NOTICE("MT8183 bl31_setup\n");

	assert(arg_from_bl2 != NULL);
	assert(arg_from_bl2->h.type == PARAM_BL31);
	assert(arg_from_bl2->h.version >= VERSION_1);

	bl32_ep_info = *arg_from_bl2->bl32_ep_info;
	bl33_ep_info = *arg_from_bl2->bl33_ep_info;
}


/*******************************************************************************
 * Perform any BL31 platform setup code
 ******************************************************************************/
void bl31_platform_setup(void)
{
	platform_setup_cpu();

	generic_delay_timer_init();

	/* Initialize the gic cpu and distributor interfaces */
	plat_gic_init();

	/* Init mcsi SF */
	plat_cci_init_sf();

#if CONFIG_SPMC_MODE == 1
	spmc_init();
#endif
	spm_boot_init();
	mcdi_init();
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
	plat_cci_init();
	plat_cci_enable();

	enable_scu(read_mpidr());

	plat_configure_mmu_el3(BL_CODE_BASE,
			       BL_COHERENT_RAM_END - BL_CODE_BASE,
			       BL_CODE_BASE,
			       BL_CODE_END,
			       BL_COHERENT_RAM_BASE,
			       BL_COHERENT_RAM_END);
}
