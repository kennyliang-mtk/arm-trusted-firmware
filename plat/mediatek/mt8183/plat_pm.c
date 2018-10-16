/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* common headers */
#include <arch_helpers.h>
#include <assert.h>
#include <debug.h>
#include <mmio.h>
#include <psci.h>
#include <errno.h>

/* mediatek platform specific headers */
#include <platform_def.h>
#include <scu.h>
#include <mtk_plat_common.h>
#include <power_tracer.h>
#include <plat_private.h>

/*******************************************************************************
 * Export the platform handlers to enable psci to invoke them
 ******************************************************************************/
static const plat_pm_ops_t plat_plat_pm_ops = {
	.affinst_standby		= NULL,
	.affinst_on			= NULL,
	.affinst_off			= NULL,
	.affinst_suspend		= NULL,
	.affinst_on_finish		= NULL,
	.affinst_suspend_finish		= NULL,
	.system_off			= NULL,
	.system_reset			= NULL,
	.get_sys_suspend_power_state	= NULL,
};

/*******************************************************************************
 * Export the platform specific power ops & initialize the mtk_platform power
 * controller
 ******************************************************************************/
int platform_setup_pm(const plat_pm_ops_t **plat_ops)
{
	*plat_ops = &plat_plat_pm_ops;
	return 0;
}
