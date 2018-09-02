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

#define SET_DEVICE_REG(lba,drv,lba_hs) (((lba)<<6) | ((drv)<<4) | ((lba_hs)&0xF))

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
}hd_cmd_t;

void init_hd();
#endif

