/*
filename:	HD.h
author:	wei-code
date:	2018-04
prupose:	硬盘驱动的相关变量及函数声明
*/

#ifndef __HD_H
#define __HD_H

#include "types.h"

/*硬盘的操作方式为先向命令块寄存器写入数据，在通过控制块寄存器发送命令*/

/*主硬盘数据端口*/
#define HD_PRI_DATA			0x1F0
/*读数据是作为错误返回寄存器，写数据时作为特征寄存器*/
#define HD_PRI_ERR_FEAT		0x1F1
/*SECTOR计数寄存器*/
#define HD_PRI_SEC_COUNT	0x1F2
/*LBA地址低字节*/
#define HD_PRI_LBA_LOW		0x1F3
/*LBA地址中字节*/
#define HD_PRI_LBA_MID		0x1F4
/*LBA地址高字节*/
#define HD_PRI_LBA_HIGH		0x1F5
/*设备寄存器，对同一个IDE通道上的主从设备通过该寄存器的第4bit来区分，0：主；1：从*/
#define HD_PRI_DEVICE		0x1F6
/*读时作为状态寄存器，写时作为命令寄存器*/
#define HD_PRI_STAT_COM		0x1F7
/*下面这个是唯一一个控制块寄存器*/
#define HD_PRI_CONTROL		0x3F6

#define SET_DEVICE_REG(lba,drv,lba_hs) (((lba)<<6) | ((drv)<<4) | ((lba_hs)&0xF))

/*SATA 指令代码*/ 
#define  SATA_RESET       		0x08	//重置reset命令
#define  SATA_RECALIBRATE       		0x10	//重新校准
#define  SATA_READ              		0x20 	//读扇区命令
#define  SATA_WRITE             		0x30 	//写扇区命令
#define  SATA_VERIFY            		0x40	//验证
#define  SATA_SEEK              		0x70	//设置读写点
#define	 SATA_EXEC_DIAGNOSE     		0x90	//硬盘诊断
#define  SATA_SET_DRIVE_PARAMETERS  	0x91	//设定硬盘参数
#define  SATA_READ_MULTIPLE     		0xC4 	//多扇区读
#define  SATA_WRITE_MULTIPLE    		0xC5 	//多扇区写
#define  SATA_SET_MULTIPLE     		0xC6	//
#define  SATA_READ_DMA          		0xC8	//DMA读
#define  SATA_WRITE_DMA              0xCA	//DMA写
#define  SATA_GET_MEDIA_STATUS       0xDA	//取设备状态
#define  SATA_STANDBY_IMMEDIATE 		0xE0 	//Standby Immediate待命
#define  SATA_IDLE_IMMEDIATE  		0xE1 	//Idle Immediate空闲
#define  SATA_SETFEATURES 			0xEF	//
#define  SATA_IDENTIFY               0xEC	//
#define  SATA_MEDIA_EJECT            0xED	//弹出


typedef struct type_hd_cmd
{
	u8 ctl;
	u8 feature;
	u8 count;
	u8 lba_low;
	u8 lba_mid;
	u8 lba_high;
	u8 device;
	u8 command;
}hd_cmd_t;

void init_hd();
#endif

