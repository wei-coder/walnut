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

#define SET_DEVICE_REG(lba,drv,lba_hs) (0xE0 | ((lba)<<6) | ((drv)<<4) | ((lba_hs)&0xF))

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
	void (*ptr_hd)();
}hd_cmd_t;

// IDE的ID命令返回的数据
// 共512字节 (256个WORD)，这里仅定义了一些感兴趣的项(基本上依据ATA/ATAPI-4)
typedef struct _IDINFO
{
	u16	wGenConfig;			//WORD 0: 基本信息字
	u16	wNumCyls;			//WORD 1: 柱面数
	u16	wReserved2;			// WORD 2: 保留
	u16	wNumHeads;			// WORD 3: 磁头数　
	u16	wReserved4;			// WORD 4: 保留
	u16	wReserved5;			// WORD 5: 保留
	u16	wNumSectorsPerTrack;		// WORD 6: 每磁 道扇区数
	u16	wVendorUnique[3];			// WORD 7-9: 厂家设定值
	char	sSerialNumber[20];		// WORD 10-19:序列号
	u16	wBufferType;				// WORD 20: 缓冲类 型
	u16	wBufferSize;				// WORD 21: 缓冲大小
	u16 wECCSize;					// WORD 22: ECC校验大小
	char	sFirmwareRev[8];		// WORD 23-26: 固件版本
	char	sModelNumber[40];		// WORD 27-46: 内部型号
	u16   wMoreVendorUnique;		// WORD 47: 厂家设定值
	u16   wReserved48;				// WORD 48: 保留
	struct{
		u16	  reserved1:8;
		u16   DMA:1;				// 1=支持DMA
		u16   LBA:1;				// 1=支持 LBA
		u16   DisIORDY:1;			// 1=可不使用IORDY
		u16   IORDY:1;				// 1=支持 IORDY
		u16   SoftReset:1;			// 1=需要ATA软启动
		u16   Overlap:1;			// 1= 支持重叠操作
		u16   Queue:1;				// 1=支持命令队列
		u16   InlDMA:1;				// 1=支持交叉存取DMA
	} wCapabilities;				// WORD 49: 一般能力
	u16   wReserved1;				// WORD 50: 保留
	u16   wPIOTiming;				// WORD 51: PIO时序
	u16   wDMATiming;				// WORD 52: DMA时序
	struct {
		u16   CHSNumber:1;			// 1=WORD 54-58有效
		u16   CycleNumber:1;		// 1=WORD 64-70有效
		u16   UnltraDMA:1;			// 1=WORD 88有效
		u16   reserved:13;
	} wFieldValidity;				// WORD 53: 后 续字段有效性标志
	u16   wNumCurCyls;				// WORD 54: CHS可寻址的柱面数
	u16   wNumCurHeads;				// WORD 55: CHS可寻址的磁头数
	u16   wNumCurSectorsPerTrack;	// WORD 56: CHS可寻址每磁道扇区数
	u16   wCurSectorsLow;			// WORD 57: CHS可寻址的扇区 数低位字
	u16   wCurSectorsHigh;			// WORD 58: CHS可寻址的扇区数高位字
	struct {
		u16   CurNumber:8;			// 当前一次性可读写扇区数
		u16   Multi:1;				// 1=已选择多扇区读写
		u16   reserved1:7;
	} wMultSectorStuff;				// WORD 59: 多 扇区读写设定
	u32		dwTotalSectors;			// WORD 60-61: LBA可寻址的扇区数
	u16		wSingleWordDMA;			// WORD 62: 单字节DMA支持能力　
	struct {
		u16   Mode0:1;				// 1=支持模式0 (4.17Mb/s)
		u16   Mode1:1;				// 1=支持模式1 (13.3Mb/s)
		u16	  Mode2:1;				// 1=支持模式2 (16.7Mb/s)
		u16   Reserved1:5;
		u16   Mode0Sel:1;			// 1=已选择模式0
		u16   Mode1Sel:1;			// 1=已选择模式1
		u16   Mode2Sel:1;			// 1=已选择模式2
		u16   Reserved2:5;
	} wMultiWordDMA;				// WORD 63: 多字节DMA支持能力
	struct {
		u16   AdvPOIModes:8;		// 支持高 级POI模式数
		u16   reserved:8;
	} wPIOCapacity;					// WORD 64: 高级PIO支持能 力
	u16   wMinMultiWordDMACycle;	// WORD 65: 多字节DMA传输周期的最小值
	u16   wRecMultiWordDMACycle;	// WORD 66: 多字节DMA传输周期的建议值
	u16   wMinPIONoFlowCycle;		// WORD 67: 无流控制时PIO传输周期的最小值
	u16   wMinPOIFlowCycle;			// WORD 68: 有流控制时PIO传输周期的最小值
	u16   wReserved69 [11];			// WORD 69-79: 保留
	struct {
		u16   Reserved1:1;
		u16   ATA1:1;				// 1=支持ATA-1
		u16   ATA2:1;				// 1=支持ATA-2
		u16   ATA3:1;				// 1=支持ATA-3
		u16   ATA4:1;				// 1=支持ATA/ATAPI-4
		u16   ATA5:1;				// 1=支持ATA/ATAPI-5
		u16   ATA6:1;				// 1=支持ATA/ATAPI-6
		u16   ATA7:1;				// 1=支持ATA/ATAPI-7
		u16   ATA8:1;				// 1=支持ATA/ATAPI- 8
		u16   ATA9:1;				// 1=支持ATA/ATAPI-9
		u16   ATA10:1;				// 1=支持 ATA/ATAPI-10
		u16   ATA11:1;				// 1=支持ATA/ATAPI-11
		u16   ATA12:1;				// 1=支持ATA/ATAPI-12
		u16   ATA13:1;				// 1=支持ATA/ATAPI-13
		u16   ATA14:1;				// 1=支持ATA/ATAPI-14
		u16   Reserved2:1;
	} wMajorVersion;				// WORD 80: 主版本
	u16   wMinorVersion;			// WORD 81: 副版本
	u16   wReserved82[6];			// WORD 82-87: 保留
	struct {
		u16   Mode0:1;				// 1=支持 模式0 (16.7Mb/s)
		u16   Mode1:1;				// 1=支持模式1 (25Mb/s)
		u16   Mode2:1;				// 1=支持模式2 (33Mb/s)
		u16   Mode3:1;				// 1=支持模式3 (44Mb/s)
		u16   Mode4:1;				// 1=支持模式4 (66Mb/s)
		u16   Mode5:1;				// 1=支持模式5 (100Mb/s)
		u16   Mode6:1;				// 1=支持模式6 (133Mb/s)
		u16   Mode7:1;				// 1=支持模式7 (166Mb/s) ???
		u16   Mode0Sel:1;			// 1=已选择模式0
		u16   Mode1Sel:1;			// 1=已选择模式1
		u16   Mode2Sel:1;			// 1=已选择模式2
		u16   Mode3Sel:1;			// 1=已选择模式3
		u16   Mode4Sel:1;			// 1=已选择模式4
		u16   Mode5Sel:1;			// 1=已选择模式5
		u16   Mode6Sel:1;			// 1=已选择模式 6
		u16   Mode7Sel:1;			// 1=已选择模式7
	} wUltraDMA;					// WORD 88:　 Ultra DMA支持能力
	u16	wReserved89[167];		// WORD 89-255
}HD_IDENTIFY_T;


void init_hd();
void identify_intr();

#endif

