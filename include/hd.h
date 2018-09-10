/*
filename:	HD.h
author:	wei-code
date:	2018-04
prupose:	Ӳ����������ر�������������
*/

#ifndef __HD_H
#define __HD_H

#include "types.h"

/*Ӳ�̵Ĳ�����ʽΪ���������Ĵ���д�����ݣ���ͨ�����ƿ�Ĵ�����������*/

/*��Ӳ�����ݶ˿�*/
#define HD_PRI_DATA			0x1F0
/*����������Ϊ���󷵻ؼĴ�����д����ʱ��Ϊ�����Ĵ���*/
#define HD_PRI_ERR_FEAT		0x1F1
/*SECTOR�����Ĵ���*/
#define HD_PRI_SEC_COUNT	0x1F2
/*LBA��ַ���ֽ�*/
#define HD_PRI_LBA_LOW		0x1F3
/*LBA��ַ���ֽ�*/
#define HD_PRI_LBA_MID		0x1F4
/*LBA��ַ���ֽ�*/
#define HD_PRI_LBA_HIGH		0x1F5
/*�豸�Ĵ�������ͬһ��IDEͨ���ϵ������豸ͨ���üĴ����ĵ�4bit�����֣�0������1����*/
#define HD_PRI_DEVICE		0x1F6
/*��ʱ��Ϊ״̬�Ĵ�����дʱ��Ϊ����Ĵ���*/
#define HD_PRI_STAT_COM		0x1F7
/*���������Ψһһ�����ƿ�Ĵ���*/
#define HD_PRI_CONTROL		0x3F6

#define SET_DEVICE_REG(lba,drv,lba_hs) (0xE0 | ((lba)<<6) | ((drv)<<4) | ((lba_hs)&0xF))

/*SATA ָ�����*/ 
#define  SATA_RESET       		0x08	//����reset����
#define  SATA_RECALIBRATE       		0x10	//����У׼
#define  SATA_READ              		0x20 	//����������
#define  SATA_WRITE             		0x30 	//д��������
#define  SATA_VERIFY            		0x40	//��֤
#define  SATA_SEEK              		0x70	//���ö�д��
#define	 SATA_EXEC_DIAGNOSE     		0x90	//Ӳ�����
#define  SATA_SET_DRIVE_PARAMETERS  	0x91	//�趨Ӳ�̲���
#define  SATA_READ_MULTIPLE     		0xC4 	//��������
#define  SATA_WRITE_MULTIPLE    		0xC5 	//������д
#define  SATA_SET_MULTIPLE     		0xC6	//
#define  SATA_READ_DMA          		0xC8	//DMA��
#define  SATA_WRITE_DMA              0xCA	//DMAд
#define  SATA_GET_MEDIA_STATUS       0xDA	//ȡ�豸״̬
#define  SATA_STANDBY_IMMEDIATE 		0xE0 	//Standby Immediate����
#define  SATA_IDLE_IMMEDIATE  		0xE1 	//Idle Immediate����
#define  SATA_SETFEATURES 			0xEF	//
#define  SATA_IDENTIFY               0xEC	//
#define  SATA_MEDIA_EJECT            0xED	//����


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

// IDE��ID����ص�����
// ��512�ֽ� (256��WORD)�������������һЩ����Ȥ����(����������ATA/ATAPI-4)
typedef struct _IDINFO
{
	u16	wGenConfig;			//WORD 0: ������Ϣ��
	u16	wNumCyls;			//WORD 1: ������
	u16	wReserved2;			// WORD 2: ����
	u16	wNumHeads;			// WORD 3: ��ͷ����
	u16	wReserved4;			// WORD 4: ����
	u16	wReserved5;			// WORD 5: ����
	u16	wNumSectorsPerTrack;		// WORD 6: ÿ�� ��������
	u16	wVendorUnique[3];			// WORD 7-9: �����趨ֵ
	char	sSerialNumber[20];		// WORD 10-19:���к�
	u16	wBufferType;				// WORD 20: ������ ��
	u16	wBufferSize;				// WORD 21: �����С
	u16 wECCSize;					// WORD 22: ECCУ���С
	char	sFirmwareRev[8];		// WORD 23-26: �̼��汾
	char	sModelNumber[40];		// WORD 27-46: �ڲ��ͺ�
	u16   wMoreVendorUnique;		// WORD 47: �����趨ֵ
	u16   wReserved48;				// WORD 48: ����
	struct{
		u16	  reserved1:8;
		u16   DMA:1;				// 1=֧��DMA
		u16   LBA:1;				// 1=֧�� LBA
		u16   DisIORDY:1;			// 1=�ɲ�ʹ��IORDY
		u16   IORDY:1;				// 1=֧�� IORDY
		u16   SoftReset:1;			// 1=��ҪATA������
		u16   Overlap:1;			// 1= ֧���ص�����
		u16   Queue:1;				// 1=֧���������
		u16   InlDMA:1;				// 1=֧�ֽ����ȡDMA
	} wCapabilities;				// WORD 49: һ������
	u16   wReserved1;				// WORD 50: ����
	u16   wPIOTiming;				// WORD 51: PIOʱ��
	u16   wDMATiming;				// WORD 52: DMAʱ��
	struct {
		u16   CHSNumber:1;			// 1=WORD 54-58��Ч
		u16   CycleNumber:1;		// 1=WORD 64-70��Ч
		u16   UnltraDMA:1;			// 1=WORD 88��Ч
		u16   reserved:13;
	} wFieldValidity;				// WORD 53: �� ���ֶ���Ч�Ա�־
	u16   wNumCurCyls;				// WORD 54: CHS��Ѱַ��������
	u16   wNumCurHeads;				// WORD 55: CHS��Ѱַ�Ĵ�ͷ��
	u16   wNumCurSectorsPerTrack;	// WORD 56: CHS��Ѱַÿ�ŵ�������
	u16   wCurSectorsLow;			// WORD 57: CHS��Ѱַ������ ����λ��
	u16   wCurSectorsHigh;			// WORD 58: CHS��Ѱַ����������λ��
	struct {
		u16   CurNumber:8;			// ��ǰһ���Կɶ�д������
		u16   Multi:1;				// 1=��ѡ���������д
		u16   reserved1:7;
	} wMultSectorStuff;				// WORD 59: �� ������д�趨
	u32		dwTotalSectors;			// WORD 60-61: LBA��Ѱַ��������
	u16		wSingleWordDMA;			// WORD 62: ���ֽ�DMA֧��������
	struct {
		u16   Mode0:1;				// 1=֧��ģʽ0 (4.17Mb/s)
		u16   Mode1:1;				// 1=֧��ģʽ1 (13.3Mb/s)
		u16	  Mode2:1;				// 1=֧��ģʽ2 (16.7Mb/s)
		u16   Reserved1:5;
		u16   Mode0Sel:1;			// 1=��ѡ��ģʽ0
		u16   Mode1Sel:1;			// 1=��ѡ��ģʽ1
		u16   Mode2Sel:1;			// 1=��ѡ��ģʽ2
		u16   Reserved2:5;
	} wMultiWordDMA;				// WORD 63: ���ֽ�DMA֧������
	struct {
		u16   AdvPOIModes:8;		// ֧�ָ� ��POIģʽ��
		u16   reserved:8;
	} wPIOCapacity;					// WORD 64: �߼�PIO֧���� ��
	u16   wMinMultiWordDMACycle;	// WORD 65: ���ֽ�DMA�������ڵ���Сֵ
	u16   wRecMultiWordDMACycle;	// WORD 66: ���ֽ�DMA�������ڵĽ���ֵ
	u16   wMinPIONoFlowCycle;		// WORD 67: ��������ʱPIO�������ڵ���Сֵ
	u16   wMinPOIFlowCycle;			// WORD 68: ��������ʱPIO�������ڵ���Сֵ
	u16   wReserved69 [11];			// WORD 69-79: ����
	struct {
		u16   Reserved1:1;
		u16   ATA1:1;				// 1=֧��ATA-1
		u16   ATA2:1;				// 1=֧��ATA-2
		u16   ATA3:1;				// 1=֧��ATA-3
		u16   ATA4:1;				// 1=֧��ATA/ATAPI-4
		u16   ATA5:1;				// 1=֧��ATA/ATAPI-5
		u16   ATA6:1;				// 1=֧��ATA/ATAPI-6
		u16   ATA7:1;				// 1=֧��ATA/ATAPI-7
		u16   ATA8:1;				// 1=֧��ATA/ATAPI- 8
		u16   ATA9:1;				// 1=֧��ATA/ATAPI-9
		u16   ATA10:1;				// 1=֧�� ATA/ATAPI-10
		u16   ATA11:1;				// 1=֧��ATA/ATAPI-11
		u16   ATA12:1;				// 1=֧��ATA/ATAPI-12
		u16   ATA13:1;				// 1=֧��ATA/ATAPI-13
		u16   ATA14:1;				// 1=֧��ATA/ATAPI-14
		u16   Reserved2:1;
	} wMajorVersion;				// WORD 80: ���汾
	u16   wMinorVersion;			// WORD 81: ���汾
	u16   wReserved82[6];			// WORD 82-87: ����
	struct {
		u16   Mode0:1;				// 1=֧�� ģʽ0 (16.7Mb/s)
		u16   Mode1:1;				// 1=֧��ģʽ1 (25Mb/s)
		u16   Mode2:1;				// 1=֧��ģʽ2 (33Mb/s)
		u16   Mode3:1;				// 1=֧��ģʽ3 (44Mb/s)
		u16   Mode4:1;				// 1=֧��ģʽ4 (66Mb/s)
		u16   Mode5:1;				// 1=֧��ģʽ5 (100Mb/s)
		u16   Mode6:1;				// 1=֧��ģʽ6 (133Mb/s)
		u16   Mode7:1;				// 1=֧��ģʽ7 (166Mb/s) ???
		u16   Mode0Sel:1;			// 1=��ѡ��ģʽ0
		u16   Mode1Sel:1;			// 1=��ѡ��ģʽ1
		u16   Mode2Sel:1;			// 1=��ѡ��ģʽ2
		u16   Mode3Sel:1;			// 1=��ѡ��ģʽ3
		u16   Mode4Sel:1;			// 1=��ѡ��ģʽ4
		u16   Mode5Sel:1;			// 1=��ѡ��ģʽ5
		u16   Mode6Sel:1;			// 1=��ѡ��ģʽ 6
		u16   Mode7Sel:1;			// 1=��ѡ��ģʽ7
	} wUltraDMA;					// WORD 88:�� Ultra DMA֧������
	u16	wReserved89[167];		// WORD 89-255
}HD_IDENTIFY_T;


void init_hd();
void identify_intr();

#endif

