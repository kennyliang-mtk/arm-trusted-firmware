/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <sgi_base_platform_def.h>
#include <utils_def.h>

#define PLAT_ARM_CLUSTER_COUNT		2
#define CSS_SGI_MAX_CPUS_PER_CLUSTER	8
#define CSS_SGI_MAX_PE_PER_CPU		2

#define PLAT_CSS_MHU_BASE		UL(0x45400000)

/* Base address of DMC-620 instances */
#define SGICLARKH_DMC620_BASE0		UL(0x4e000000)
#define SGICLARKH_DMC620_BASE1		UL(0x4e100000)

#define PLAT_MAX_PWR_LVL		ARM_PWR_LVL2

#define CSS_SYSTEM_PWR_DMN_LVL		ARM_PWR_LVL3

#endif /* PLATFORM_DEF_H */
