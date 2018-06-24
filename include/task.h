/*
filename:	task.h
author:		wei-coder
date:		2018-1
purpose:	�̡߳�������ص����ݽṹ����
*/

#ifndef __TASK_H
#define __TASK_H

#include "types.h"
#include "signal.h"

/*����״̬*/
#define	TASK_UNNABLE	-1		//���񲻿�����
#define TASK_RUNNING	0		// �����������л���׼��������
#define TASK_INTERRUPTIBLE 1	// ���̴��ڿ��жϵȴ�״̬��
#define TASK_UNINTERRUPTIBLE 2	// ���̴��ڲ����жϵȴ�״̬����Ҫ����I/O �����ȴ���
#define TASK_ZOMBIE 3			// ���̴��ڽ���״̬���Ѿ�ֹͣ���У��������̻�û���źš�
#define TASK_STOPPED 4			// ������ֹͣ��

/*�ں��̵߳��������л��������Ϣ*/
struct context
{
	u32	esp;		//��ջָ��
	u32	ebp;		//ջ��ָ��
	u32	ebx;		//��ַ�Ĵ���
	u32	esi;		//Դ����
	u32	edi;		//Ŀ������
	u32	eflags;		//״̬�Ĵ���
};

// ����״̬�����ݽṹ
typedef struct tss_struct
{
	long back_link;		/* 16 high bits zero */
	long esp0;
	long ss0;				/* 16 high bits zero */
	long esp1;
	long ss1;				/* 16 high bits zero */
	long esp2;
	long ss2;				/* 16 high bits zero */
	long cr3;
	long eip;
	long eflags;
	long eax;
	long ecx;
	long edx;
	long ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es;			/* 16 high bits zero */
	long cs;			/* 16 high bits zero */
	long ss;			/* 16 high bits zero */
	long ds;			/* 16 high bits zero */
	long fs;			/* 16 high bits zero */
	long gs;			/* 16 high bits zero */
	long ldt;
	long io_bitmap;
}tss_struct;


/*���̿��ƿ�*/
typedef struct task_struct
{
	long state;						// long state ���������״̬��-1 �������У�0 ������(����)��>0 ��ֹͣ����
	long counter;					// long counter ��������ʱ�����(�ݼ�)���δ�����������ʱ��Ƭ��
	long priority;					// ����������������ʼ����ʱcounter = priority��Խ������Խ����
	long signal;					//�źš���λͼ��ÿ������λ����һ���źţ��ź�ֵ=λƫ��ֵ+1��
	struct sigaction sigaction[32];	//�ź�ִ�����Խṹ����Ӧ�źŽ�Ҫִ�еĲ����ͱ�־��Ϣ��
	long blocked;					//�����ź������루��Ӧ�ź�λͼ����
	
	int exit_code;					//����ִ��ֹͣ���˳��룬�丸���̻�ȡ��
	ulong start_code;				//����ε�ַ��
	ulong end_code;					//���볤�ȣ��ֽ�������
	ulong end_data;					//���볤�� + ���ݳ��ȣ��ֽ�������
	ulong brk;						//�ܳ��ȣ��ֽ�������
	ulong start_stack;				//��ջ�ε�ַ��
	long pid;						//���̱�ʶ��(���̺�)��
	long father;					//�����̺š�
	long pgrp;						//��������š�
	long session;					//�Ự�š�
	long leader;					//�Ự���졣
	u16	uid;						//�û���ʶ�ţ��û�id����
	u16	euid;						//��Ч�û�id��
	u16	suid;						//������û�id��
	u16 gid;						//���ʶ�ţ���id����
	u16	egid;						//��Ч��id��
	u16 sgid;						//�������id��
	long alarm;						//������ʱֵ���δ�������
	long utime;						//�û�̬����ʱ�䣨�δ�������
	long stime;						//ϵͳ̬����ʱ�䣨�δ�������
	long cutime;					//�ӽ����û�̬����ʱ�䡣
	long cstime;					//�ӽ���ϵͳ̬����ʱ�䡣
	long start_time;				//���̿�ʼ����ʱ�̡�
	u16 used_math;					//��־���Ƿ�ʹ����Э��������
	
/* file system info */
	int tty;						//����ʹ��tty �����豸�š�-1 ��ʾû��ʹ�á�
	u16 umask;						//�ļ�������������λ��
	u32 *pwd;						//��ǰ����Ŀ¼i �ڵ�ṹ�� ��ʱͨ����ָͨ�����
	u32 *root;						//��Ŀ¼i �ڵ�ṹ����ʱͨ����ָͨ�����
	u32 *executable;				//ִ���ļ�i �ڵ�ṹ����ʱͨ����ָͨ�����
	ulong close_on_exec;			//ִ��ʱ�ر��ļ����λͼ��־�����μ�include/fcntl.h��
	u32	pdt;
	long esp;
	long esp0;
	long eip;

	//struct desc_t ldt[3];			//������ľֲ�����������0-�գ�1-�����cs��2-���ݺͶ�ջ��ds&ss��
	//tss_struct tss;				//�����̵�����״̬����Ϣ�ṹ��
}task_struct;

#endif
