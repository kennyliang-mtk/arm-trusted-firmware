/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAT_PARAMS_H
#define PLAT_PARAMS_H

#include <stdint.h>

#define BL31_GPIO_LEVEL_LOW	0
#define BL31_GPIO_LEVEL_HIGH	1

/* param type */
enum {
	PARAM_RESET,
	PARAM_COREBOOT_TABLE,
};

struct gpio_info {
	uint32_t index;
	uint8_t polarity;
};

/* common header for all plat parameter type */
struct bl31_plat_param {
	uint64_t type;
	void *next;
};

struct bl31_gpio_param {
	struct bl31_plat_param h;
	struct gpio_info gpio;
};

struct bl31_u64_param {
	struct bl31_plat_param h;
	uint64_t value;
};

#endif
