/*
filename:	hardware.h
author:	wei-code
date:	2018-11
prupose:系统硬件处理相关数据结构
*/

#pragma once

#include <types.h>
#include <klib.h>

#define PCI_CONFIG_ADDRESS		0xCF8
#define PCI_CONFIG_DATA			0xCFC

struct dev_conf_addr
{
	u8	dvcd_enable:1;
	u8	dvcd_reserve:7;
	u8	dvcd_busno:8;
	u8	dvcd_devno:5;
	u8	dvcd_funcno:3;
	u8	dvcd_regno:6;
	u8	dvcd_ro:2;
};

struct dev_conf_data
{
	u16	dcd_vendorid;
	u16 dcd_devid;
	u16 dcd_comreg;
	u16	dcd_statusreg;
	u16 dcd_reviid;
	u32	dcd_clacod:24;
	u8	dcd_cachline;
	u8	dcd_latentimer;
	u8	dcd_bist;
	u32	dcd_bar0;
	u32	dcd_bar1;
	u32	dcd_bar2;
	u32	dcd_bar3;
	u32	dcd_bar4;
	u32	dcd_bar5;
	u32	dcd_cisp;
	u16	dcd_subsys_vendid;
	u16	dcd_subsys_devid;
	u32	dcd_exrombasead;
	u32 dcd_reseved1;
	u32 dcd_reseved2;
	u8	dcd_irqline;
	u8	dcd_irqpin;
	u8	dcd_mingnt;
	u8	dcd_maxlat;
};

struct pci_device
{
	struct list_head	devNode;
	struct dev_conf_addr	devcaddr;
	struct dev_conf_data	devcdata;
};

struct pci_info
{
	int devnum;
	struct pci_device * devlist;
};