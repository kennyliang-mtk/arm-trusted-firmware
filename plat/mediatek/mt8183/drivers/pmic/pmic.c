/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pmic_wrap_init.h>
#include <pmic.h>

void pmic_power_off(void)
{
	pwrap_write(PMIC_PWRHOLD, 0x0);
}
