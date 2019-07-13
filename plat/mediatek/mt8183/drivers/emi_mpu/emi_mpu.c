/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <lib/mmio.h>
#include <emi_mpu.h>

struct emi_mpu_vio_info {
	uint32_t dbg_s;
	uint32_t dbg_t;
	uint32_t master_ID;
	uint32_t domain_ID;
	uint32_t region;
	uint32_t dbg_pqry;
	uint32_t reserved0;
	uint32_t reserved1;
};

static struct emi_mpu_vio_info emi_mpu_vio_info;

int is_4GB(void)
{
	return 0; /* 8183 doesn't use 4GB */
}

/*
 * emi_mpu_set_region_protection: protect a region.
 * @start: start address of the region
 * @end: end address of the region
 * @region: EMI MPU region id
 * @access_permission: EMI MPU access permission
 * Return 0 for success, otherwise negative status code.
 */
int emi_mpu_set_region_protection(
	unsigned long start, unsigned long end,
	int region,
	unsigned int access_permission)
{
	int ret = 0;

	if (end <= start) {
		ERROR("[EMI][MTEE][MPU] Invalid address!.\n");
		return -1;
	}

	if (is_4GB()) {
		/* 4GB mode: emi_addr = phy_addr & 0xffff */
		start = EMI_PHY_OFFSET & 0xffff;
		end = EMI_PHY_OFFSET & 0xffff;
	} else {
		/* non-4GB mode: emi_addr = phy_addr - MEM_OFFSET */
		start = start - EMI_PHY_OFFSET;
		end = end - EMI_PHY_OFFSET;
	}

	/*Address 64KB alignment*/
	start = start >> 16;
	end = end >> 16;

	switch (region) {
	case 0:
		mmio_write_32(EMI_MPU_APC0, 0);
		mmio_write_32(EMI_MPU_SA0, start);
		mmio_write_32(EMI_MPU_EA0, end);
		mmio_write_32(EMI_MPU_APC0, access_permission);
		break;

	case 1:
		mmio_write_32(EMI_MPU_APC1, 0);
		mmio_write_32(EMI_MPU_SA1, start);
		mmio_write_32(EMI_MPU_EA1, end);
		mmio_write_32(EMI_MPU_APC1, access_permission);
		break;

	case 2:
		mmio_write_32(EMI_MPU_APC2, 0);
		mmio_write_32(EMI_MPU_SA2, start);
		mmio_write_32(EMI_MPU_EA2, end);
		mmio_write_32(EMI_MPU_APC2, access_permission);
		break;

	case 3:
		mmio_write_32(EMI_MPU_APC3, 0);
		mmio_write_32(EMI_MPU_SA3, start);
		mmio_write_32(EMI_MPU_EA3, end);
		mmio_write_32(EMI_MPU_APC3, access_permission);
		break;

	case 4:
		mmio_write_32(EMI_MPU_APC4, 0);
		mmio_write_32(EMI_MPU_SA4, start);
		mmio_write_32(EMI_MPU_EA4, end);
		mmio_write_32(EMI_MPU_APC4, access_permission);
		break;

	case 5:
		mmio_write_32(EMI_MPU_APC5, 0);
		mmio_write_32(EMI_MPU_SA5, start);
		mmio_write_32(EMI_MPU_EA5, end);
		mmio_write_32(EMI_MPU_APC5, access_permission);
		break;

	case 6:
		mmio_write_32(EMI_MPU_APC6, 0);
		mmio_write_32(EMI_MPU_SA6, start);
		mmio_write_32(EMI_MPU_EA6, end);
		mmio_write_32(EMI_MPU_APC6, access_permission);
		break;

	case 7:
		mmio_write_32(EMI_MPU_APC7, 0);
		mmio_write_32(EMI_MPU_SA7, start);
		mmio_write_32(EMI_MPU_EA7, end);
		mmio_write_32(EMI_MPU_APC7, access_permission);
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

/* Clear EMI MPU violation*/
static void clear_emi_mpu_vio(void)
{
	uint32_t dbg_s, dbg_t, dbg_pqry, wr_vio, OOR_VIO;
	uint32_t master_ID, domain_ID;
	uint32_t region;

	dbg_s = mmio_read_32(EMI_MPUS);
	dbg_t = mmio_read_32(EMI_MPUT);

	master_ID = (dbg_s & 0x0000FFFF);
	domain_ID = (dbg_s >> 21) & 0xf;
	region = (dbg_s >> 16) & 0x1f;
	wr_vio = (dbg_s >> 29) & 0x3;
	OOR_VIO = (dbg_s >> 27) & 0x3;

	switch (domain_ID) {
	case 0:
		dbg_pqry = mmio_read_32(EMI_MPUP);
	break;
	case 1:
		dbg_pqry = mmio_read_32(EMI_MPUQ);
	break;
	case 2:
		dbg_pqry = mmio_read_32(EMI_MPUR);
	break;
	case 3:
		dbg_pqry = mmio_read_32(EMI_MPUY);
	break;
	default:
		dbg_pqry = 0;
	break;
	}
	emi_mpu_vio_info.dbg_s = dbg_s;
	emi_mpu_vio_info.dbg_t = dbg_t;
	emi_mpu_vio_info.master_ID = master_ID;
	emi_mpu_vio_info.domain_ID = domain_ID;
	emi_mpu_vio_info.region = region;
	emi_mpu_vio_info.dbg_pqry = dbg_pqry;
	emi_mpu_vio_info.reserved0 = 0;
	emi_mpu_vio_info.reserved1 = 0;

	ERROR("[EMI] [MTEE][EMI MPU] Debug info start ----------\n");
	ERROR("[EMI] [MTEE]EMI_MPUS = %x, EMI_MPUT = %x.\n", dbg_s, dbg_t);
	ERROR("[EMI] [MTEE]Violation address is 0x%x.\n", dbg_t + 0x40000000);
	ERROR("[EMI] [MTEE]Violation master ID is 0x%x.\n", master_ID);
	ERROR("[EMI] [MTEE]Violation domain ID is 0x%x.\n", domain_ID);
	ERROR("[EMI] [MTEE]%s violation.\n", (wr_vio == 1) ?  "Write" : "Read");
	ERROR("[EMI] [MTEE]Corrupted region is %d\n\r", region);
	if (OOR_VIO != 0)
		ERROR("[EMI] [MTEE]%s Out of range violation.\n",
			(OOR_VIO == 1) ?	"Write" : "Read");
	ERROR("[EMI] [MTEE][EMI MPU] Debug info end-------------\n");

	/* clear violation status */
	mmio_write_32(EMI_MPUD0_ST, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD1_ST, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD2_ST, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD3_ST, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD0_ST2, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD1_ST2, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD2_ST2, 0xFFFFFFFF);
	mmio_write_32(EMI_MPUD3_ST2, 0xFFFFFFFF);

	/* clear debug info */
	mmio_write_32(EMI_MPUS, 0x80000000);
	dbg_s = mmio_read_32(EMI_MPUS);
	dbg_t = mmio_read_32(EMI_MPUT);

	if (dbg_s) {
		ERROR("[EMI] [MTEE] Fail to clear EMI MPU violation\n");
		ERROR("[EMI] [MTEE] EMI_MPUS = %x, EMI_MPUT = %x",
			dbg_s, dbg_t);
	}
}

void dump_emi_mpu_regions()
{
	unsigned int apc, sa, ea;
	unsigned int apc_addr = EMI_MPU_APC0, sa_addr = EMI_MPU_SA0, ea_addr = EMI_MPU_EA0;
	int i;

	for(i = 0; i < 8; ++i) {
		apc = mmio_read_32(apc_addr + i * 4);
		sa = mmio_read_32(sa_addr + i * 4);
		ea = mmio_read_32(ea_addr + i * 4);
		WARN("region %d:\n", i);
		WARN("\tapc:0x%x, sa:0x%x, ea:0x%x\n", apc, sa, ea);
	}
}
