/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PMIC_H__
#define __PMIC_H__

enum {
	PMIC_PWRHOLD = 0x0a08,
};

/* external API */
void pmic_power_off(void);

#endif /* __PMIC_H__ */
