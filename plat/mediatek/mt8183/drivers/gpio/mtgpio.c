/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <debug.h>
#include <mmio.h>
#include <platform_def.h>
#include <mcucfg.h>
#include <spm.h>
#include <delay_timer.h>
#include <gpio/mtgpio.h>
#include <gpio/mtgpio_cfg.h>
#include <gpio.h>

/******************************************************************************
 MACRO Definition
******************************************************************************/
//#define  GIO_SLFTEST
#define VERSION     GPIO_DEVICE
/*---------------------------------------------------------------------------*/
#define REGSET (4)
#define REGCLR (8)
#define REGMWR (0xC)
#define GPIO_MODE_BITS                 4
#define MAX_GPIO_MODE_PER_REG          8
#define MAX_GPIO_REG_BITS              32
/*---------------------------------------------------------------------------*/
#define TRUE                            1
#define FALSE                           0

#define GIO_INVALID_OBJ(ptr)            ((ptr) != gpio_obj)
/******************************************************************************
Enumeration/Structure
******************************************************************************/
/*---------------------------------------------------------------------------*/
void mt_set_gpio_dir_chip(uint32_t pin, int dir)
{
	uint32_t bit;

	assert(pin < MAX_GPIO_PIN);

	assert(dir < GPIO_DIR_MAX);

	bit = pin % MAX_GPIO_REG_BITS;

	/* To reduce memory usage, we don't use DIR_addr[] array*/
	if (dir == GPIO_DIR_IN)
		mmio_write_32(DIR_addr[pin].addr + 0x8, 1U << bit);
	else
		mmio_write_32(DIR_addr[pin].addr + 0x4, 1U << bit);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_dir_chip(uint32_t pin)
{
	uint32_t bit;
	uint32_t reg;

	assert(pin < MAX_GPIO_PIN);

	bit = pin % MAX_GPIO_REG_BITS;

	/* To reduce memory usage, we don't use DIR_addr[] array*/
	reg = mmio_read_32(DIR_addr[pin].addr);
	return (((reg & (1L << bit)) != 0) ? GPIO_DIR_OUT : GPIO_DIR_IN);
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_out_chip(uint32_t pin, int output)
{
	uint32_t bit;

	assert(pin < MAX_GPIO_PIN);

	assert(output < GPIO_OUT_MAX);

	bit = pin % MAX_GPIO_REG_BITS;

	/* To reduce memory usage, we don't use DATAOUT_addr[] array*/
	if (output == GPIO_OUT_ZERO)
		mmio_write_32(DATAOUT_addr[pin].addr + 0x8, 1U << bit);
	else
		mmio_write_32(DATAOUT_addr[pin].addr + 0x4, 1U << bit);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_out_chip(uint32_t pin)
{
	uint32_t bit;
	uint32_t reg;

	assert(pin < MAX_GPIO_PIN);

	bit = pin % MAX_GPIO_REG_BITS;

	/* To reduce memory usage, we don't use DATAOUT_addr[] array*/
	reg = mmio_read_32(DATAOUT_addr[pin].addr);
	return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_in_chip(uint32_t pin)
{
	uint32_t bit;
	uint32_t reg;

	assert(pin < MAX_GPIO_PIN);

	bit = pin % MAX_GPIO_REG_BITS;

	/* To reduce memory usage, we don't use DIN_addr[] array*/
	reg = mmio_read_32(DIN_addr[pin].addr);
	return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_mode_chip(uint32_t pin, int mode)
{
	uint32_t bit;
	uint32_t data;
	uint32_t mask;


	assert(pin < MAX_GPIO_PIN);

	assert(mode < GPIO_MODE_MAX);

	mask = (1L << GPIO_MODE_BITS) - 1;
	mode = mode & mask;
	bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;

	data = mmio_read_32(MODE_addr[pin].addr);
	data &= (~(mask << bit));
	data |= (mode << bit);
	mmio_write_32(MODE_addr[pin].addr, data);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_mode_chip(uint32_t pin)
{
	uint32_t bit;
	uint32_t data;
	uint32_t mask;

	assert(pin < MAX_GPIO_PIN);

	mask = (1L << GPIO_MODE_BITS) - 1;

	bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;
	data = mmio_read_32(MODE_addr[pin].addr);
	return (data >> bit) & mask;
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_pull_enable_chip(uint32_t pin, int en)
{
	assert(pin < MAX_GPIO_PIN);

	assert(!((PULLEN_offset[pin].offset == (int8_t)-1) && (PUPD_offset[pin].offset == (int8_t)-1)));

	if (en == GPIO_PULL_DISABLE) {
		if (PULLEN_offset[pin].offset == (int8_t)-1)
			mmio_clrbits_32(PUPD_addr[pin].addr, 3L << PUPD_offset[pin].offset);
		else
			mmio_clrbits_32(PULLEN_addr[pin].addr, 1L << PULLEN_offset[pin].offset);
	} else if (en == GPIO_PULL_ENABLE) {
		if (PULLEN_offset[pin].offset == (int8_t)-1) {
			/* For PUPD+R0+R1 Type, mt_set_gpio_pull_enable does not know
			 * which one between PU and PD shall be enabled.
			 * Use R0 to guarantee at one resistor is set when lk
			 * apply default setting
			 */
			mmio_setbits_32(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
			mmio_clrbits_32(PUPD_addr[pin].addr, 1L << (PUPD_offset[pin].offset + 1));
		} else {
			/* For PULLEN + PULLSEL Type */
			mmio_setbits_32(PULLEN_addr[pin].addr, 1L << PULLEN_offset[pin].offset);
		}
	} else if (en == GPIO_PULL_ENABLE_R0) {
		assert(!(PUPD_offset[pin].offset == (int8_t)-1));
		/* IOConfig does not support MWR operation, so use CLR + SET */
		mmio_setbits_32(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
		mmio_clrbits_32(PUPD_addr[pin].addr, 1L << (PUPD_offset[pin].offset + 1));
	} else if (en == GPIO_PULL_ENABLE_R1) {
		assert(!(PUPD_offset[pin].offset == (int8_t)-1));
		/* IOConfig does not support MWR operation, so use CLR + SET */
		mmio_clrbits_32(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
		mmio_setbits_32(PUPD_addr[pin].addr, 1L << (PUPD_offset[pin].offset + 1));
	} else if (en == GPIO_PULL_ENABLE_R0R1) {
		assert(!(PUPD_offset[pin].offset == (int8_t)-1));
		mmio_setbits_32(PUPD_addr[pin].addr, 3L << PUPD_offset[pin].offset);
	}
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull_enable_chip(uint32_t pin)
{
	uint32_t reg;

	assert(pin < MAX_GPIO_PIN);

	assert(!((PULLEN_offset[pin].offset == (int8_t)-1) && (PUPD_offset[pin].offset == (int8_t)-1)));

	if (PULLEN_offset[pin].offset == (int8_t)-1) {
		reg = mmio_read_32(PUPD_addr[pin].addr);
		return ((reg & (3U << PUPD_offset[pin].offset)) ? 1 : 0);
	} else if (PUPD_offset[pin].offset == (int8_t)-1) {
		reg = mmio_read_32(PULLEN_addr[pin].addr);
		return ((reg & (1U << PULLEN_offset[pin].offset)) ? 1 : 0);
	}

	return -ERINVAL;
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_pull_select_chip(uint32_t pin, int sel)
{
	assert(pin < MAX_GPIO_PIN);

	assert(!((PULLSEL_offset[pin].offset == (int8_t) -1) && (PUPD_offset[pin].offset == (int8_t)-1)));

	if (sel == GPIO_PULL_NONE) {
		/*  Regard No PULL as PULL disable + pull down */
		mt_set_gpio_pull_enable_chip(pin, GPIO_PULL_DISABLE);
		if (PULLSEL_offset[pin].offset == (int8_t)-1)
			mmio_setbits_32(PUPD_addr[pin].addr, 1U << (PUPD_offset[pin].offset + 2));
		else
			mmio_clrbits_32(PULLSEL_addr[pin].addr, 1U << PULLSEL_offset[pin].offset);
	} else if (sel == GPIO_PULL_UP) {
		mt_set_gpio_pull_enable_chip(pin, GPIO_PULL_ENABLE);
		if (PULLSEL_offset[pin].offset == (int8_t)-1)
			mmio_clrbits_32(PUPD_addr[pin].addr, 1U << (PUPD_offset[pin].offset + 2));
		else
			mmio_setbits_32(PULLSEL_addr[pin].addr, 1U << PULLSEL_offset[pin].offset);
	} else if (sel == GPIO_PULL_DOWN) {
		mt_set_gpio_pull_enable_chip(pin, GPIO_PULL_ENABLE);
		if (PULLSEL_offset[pin].offset == -1)
			mmio_setbits_32(PUPD_addr[pin].addr, 1U << (PUPD_offset[pin].offset + 2));
		else
			mmio_clrbits_32(PULLSEL_addr[pin].addr, 1U << PULLSEL_offset[pin].offset);
	}
}
/*---------------------------------------------------------------------------*/
/* get pull-up or pull-down, regardless of resistor value */
int mt_get_gpio_pull_select_chip(uint32_t pin)
{
	uint32_t reg;

	assert(pin < MAX_GPIO_PIN);

	assert(!((PULLSEL_offset[pin].offset == (int8_t)-1) && (PUPD_offset[pin].offset == (int8_t)-1)));

	if (PULLEN_offset[pin].offset == (int8_t)-1) {
		reg = mmio_read_32(PUPD_addr[pin].addr);
		if (reg & (3U << PUPD_offset[pin].offset)) {
			reg = mmio_read_32(PUPD_addr[pin].addr);
			/* Reg value: 0 for PU, 1 for PD --> reverse return value */
			return ((reg & (1U << (PUPD_offset[pin].offset+2))) ? GPIO_PULL_DOWN : GPIO_PULL_UP);
		} else {
			return GPIO_PULL_NONE;
		}
	} else if (PUPD_offset[pin].offset == (int8_t)-1) {
		reg = mmio_read_32(PULLEN_addr[pin].addr);
		if ((reg & (1U << PULLEN_offset[pin].offset))) {
			reg = mmio_read_32(PULLSEL_addr[pin].addr);
			return ((reg & (1U << PULLSEL_offset[pin].offset)) ? GPIO_PULL_UP : GPIO_PULL_DOWN);
		} else {
			return GPIO_PULL_NONE;
		}
	}

	return -ERINVAL;
}
/*---------------------------------------------------------------------------*/
void  mt_gpio_pin_decrypt(int *cipher)
{
	//just for debug, find out who used pin number directly
	if ((*cipher & (0x80000000)) == 0) {
		;
	}

	*cipher &= ~(0x80000000);
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_dir(int gpio, int direction)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	mt_set_gpio_dir_chip(pin, direction);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_dir(int gpio)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	return mt_get_gpio_dir_chip(pin);
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void mt_set_gpio_pull(int gpio, int pull)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	mt_set_gpio_pull_select_chip(pin, pull);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull(int gpio)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	return mt_get_gpio_pull_select_chip(pin);
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_out(int gpio, int value)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	mt_set_gpio_out_chip(pin, value);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_out(int gpio)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	return mt_get_gpio_out_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_in(int gpio)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	return mt_get_gpio_in_chip(pin);
}
/*---------------------------------------------------------------------------*/
void mt_set_gpio_mode(int gpio, int mode)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	mt_set_gpio_mode_chip(pin, mode);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_mode(int gpio)
{
	uint32_t pin;

	mt_gpio_pin_decrypt(&gpio);
	pin = (uint32_t)gpio;
	return mt_get_gpio_mode_chip(pin);
}

const gpio_ops_t mtgpio_ops = {
	 .get_direction = mt_get_gpio_dir,
	 .set_direction = mt_set_gpio_dir,
	 .get_value = mt_get_gpio_in,
	 .set_value = mt_set_gpio_out,
	 .set_pull = mt_set_gpio_pull,
	 .get_pull = mt_get_gpio_pull,
};

