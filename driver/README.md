�ο�:
http://www.verysource.com/code/9693198_1/IDE.h.html
https://max.book118.com/html/2014/1013/9848872.shtm
https://blog.csdn.net/u011164819/article/details/52043678
http://blog.chinaunix.net/uid-20235103-id-1970841.html
https://wenda.chinawjzx.com/ztnews/idfdce5dbcjh657ij8gide89.html

hd�Ĵ���:

COMMAND: ��ʱ״̬��дʱ����
	7---------6---------5---------4------------3------------2---------------1---------------0
	BSY		DRDY	  DF/SE       #           DRQ			#				ERR
	bit7: 0:���� 1:æ
	bit6  0:û��׼���� 1 ׼����


DEVICE: 
	7--------6---------5---------4------------3------------2---------------1---------------0
	1	    L/C		   1		DEV
	bit 6 0:chs  1:LAB
	bit 4 0:master 1:slave

SEC_COUNT:
	��ȡ��������

LBA_LOW/MID/HIGH:
	0,0,0 LBA:0
	CHS: 0��ͷ��0�ŵ���0����

ѭ����ȡbit7��6,��� Ϊ01��������ִ����ϡ�
��ʱ���bit0 Ϊ1��ִ�г�������ɹ��������������� HD_PRI_ERR_FEAT.

����Ĵ���:

λ����
0  AMNF��û�ҵ���Ҫ���ʵ���������������
1 TK0NF����ִ�лָ�RECALIBRATE����ʱ��0�ŵ�û�з��֡�
2 ABRT����Ӳ�̷��Ƿ�ָ�����Ӳ�����������϶��������ִ�е��жϡ�
3 MAC�����ź���������������֪ͨ����ʾ���ʵĸı䡣
4 IDNF��û���ҵ����ʵ���������CRC��������
5 MC�����Ƿ��͸�����һ���ź���֪ͨ����ʹ���µĴ�����ʡ�
6 UNC���ڶ���������ʱ���ֲ���У����ECC������˴˴����ݴ�����Ч��
7  BBK���ڷ���������ID���ݳ����ֻ������ݿ�ʱ����1

���������Ĵ�����ָ����Ҫ��д����������������0��ʾ����256����������������ݶ�д���̷������󣬼Ĵ�����������δ��д��������Ŀ��  

�ŵ����Ĵ�����ָ����Ҫ��д�Ĵŵ����� 

��������ͷ�Ĵ�����ָ��Ӳ�������������ͷ�ź�Ѱַ��ʽ�����4��ʾ��
7 6 5  4   3    2     1   0 
1 L 1 DRV  HS3  HS2  HS1 HS0  

��5  IDE״̬�Ĵ���λ����
0  ERR������(ERROR)����λΪ1��ʾ�ڽ���ǰ�ε�����ִ��ʱ�������޷��ָ��Ĵ����ڴ���Ĵ����б����˸���Ĵ�����Ϣ��
1 IDX����ӳ������������������źš�
2 CORR����λΪ1ʱ����ʾ�Ѱ�ECC�㷨У��Ӳ�̵Ķ����ݡ�
3 DRQ��Ϊ1��ʾ���������������ݴ���(����д)��
4 DSC��Ϊ1��ʾ��ͷ���Ѱ����������ͣ���ڸõ��ϡ�
5 DF��Ϊ1ʱ����ʾ����������д���ϡ�
6 DRDY��Ϊ1ʱ��ʾ������׼���ã����Խ������
7  BSY��Ϊ1ʱ��ʾ������æ(BSY)������ִ������ڷ�������ǰ���жϸ�λ��

����Ĵ���������ִ�е�������롣��������Ĵ���д����ʱ����ظ�����Ĳ���������д�롣��д����ʱ��״̬�Ĵ�����BSYλ��1����������ǷǷ�������ִֹ�С�
��ATA��׼�У�IDE����һ����30�����������10����ͨ����(Ҳ��ǿ����)�����Ҫ�Ĳ������6��ʾ�����е�Wordָ2���ֽڡ�

Word 1 		Word 3 		Word 6  			Word 10-19 			Word 60-61  

�ŵ���   	��ͷ��	ÿ�ŵ���������		20��ASCII��ϵ�к�	LBA����Ѱ�ҵ����������

20H  ����������(������)����Ӳ�̶�ȡָ���ŵ�����ͷ�ϵ�1~256���������������͵����������ݿ������4���ֽڵ�ECCУ���룬
������ʼ�����ź����������������ָ������������Ҳ������Ѱ��ָ���Ĵŵ��ţ����Բ���Ҫ�����Ѱ�����

30H  д��������(������)���������ǽ������ڵ�����д��Ӳ�̣�����дָ���ŵ�����ͷ�ϵ�1~256����������������������ƣ�
��������Ҳ������Ѱ��ָ���Ĵŵ��ţ�д����ʼ�����ź����������������ָ����

90H  Ӳ�����������ж�Ӳ���Ƿ��Ѿ����ӵ������ϣ����Զ�ȡ����Ĵ����Լ����Ҫ�Ľ���������01H��81H��ʾ�豸�Ǻõģ�
�����ʾ�豸û�����ӻ��豸�ǻ��ġ�

�豸���ƼĴ��������üĴ�����SRSTλ����Ϊ1������ʹӲ�����������ڸ�λ״̬��IEN��ʾ�Ƿ������жϣ�����0Ϊ�����ɴ˿ɼ���
�ԸüĴ�������0X0CH�����Ӳ�̸�λ�����ʽ���7��ʾ��

  ��7  IDE   �豸���ƼĴ���

7 6 5 4 3 	2 	1 		0 
- - - - 1 SRST  IEN  	0  


  LBA=(�����*��ͷ��+��ͷ��)*������+�������

  PIO��ʽ�������ִ�й������£� 
1)  ����Ҫ��������λ�ã�����ƼĴ���1F2H~1F6H���������������������״̬�Ĵ���1F7H��DRDY��λ�������һ���� 
2)  ���������������������1F7H���Ͷ�����20H�� 
3)  ����������״̬�Ĵ���1F7H�е�BSYλ���������ݷ��͵�Ӳ�̻������� 
4)  ��������ȡһ���������Զ�����״̬�Ĵ���1F7H��DRQ��������λ�������BSYλæ�źš� DRQλ֪ͨ�������ڿ��Դӻ�
	�����ж�ȡ512�ֽڻ����(�����READ LONG COMMAND����)�����ݣ�ͬʱ��������INTRQ�ж������źţ� 
5)  ������Ӧ�ж����󣬿�ʼ��ȡ״̬�Ĵ���1F7H�����ж϶�����ִ�е������ͬʱ���������INTRQ�ж������źţ� 
6)  ����״̬�Ĵ����������ȡ����������ִ������������7)������д��󣬽�������������ECC�����ٶ�ȡһ�Σ������˳��������У� 
7)  ����ͨ�����ݼĴ���1F0H��ȡӲ�̻������е����ݵ������������У���һ���������ݱ����꣬����������-1�����������������Ϊ0������3)���������8)�� 
8)  �����е��������������ݱ���ȡ������ִ�н����� 

PIO��ʽд�����ִ�й������£� 
1)  ����Ҫд������λ�ã������������ƼĴ���1F2H~1F6H���������������������DRDY��λ�������һ���� 
2)  ���������������������1F7H����д����30H�� 
3)  ��������״̬�Ĵ���1F7H������DRQ���������źţ� 
4)  ����ͨ�����ݼĴ���1F0H��ָ���ڴ�(BUF)�е����ݴ��䵽�������� 
5)  ����������������������512���ֽڵ����ݺ�����������״̬�Ĵ���1F7H�е�BSYλ�������DRQ���������źţ� 
6)  �������е����ݿ�ʼ��д����������ָ���������У�һ��������һ���������������������BSY�źţ�ͬʱ����INTRQ�� 
7)  ������ȡ��������״̬1F7H�ʹ���Ĵ���1F1H�����ж�д����ִ�е������������޷��˷��Ĵ���(�绵��)���˳������򣬷��򣬽�����һ���� 
8)  ���������������д����������3)���򣬽�����һ����   
9)  �����е��������������ݱ�д������ִ�н����� 
	��ȻӲ�̻������������ɺܶ����������ÿ��/дһ�����������ж�������ִ�е�״̬�Ĵ����������Ϳ��Ա�֤��д�����ݵ���ȷ�ԡ� 


*ATA ָ�����*/ 
#define  IDE_CMD_ATAPI_RESET       		0x08	//����reset����
#define  IDE_CMD_RECALIBRATE       		0x10	//����У׼
#define  IDE_CMD_READ              		0x20 	//����������
#define  IDE_CMD_WRITE             		0x30 	//д��������
#define  IDE_CMD_VERIFY            		0x40	//��֤
										0x50	//format
										0x60	//init
#define  IDE_CMD_SEEK              		0x70	//���ö�д��
#define	 IDE_CMD_EXEC_DIAGNOSE     		0x90	//Ӳ�����
#define  IDE_CMD_SET_DRIVE_PARAMETERS  	0x91	//�趨Ӳ�̲���
#define  IDE_CMD_READ_MULTIPLE     		0xC4 	//��������
#define  IDE_CMD_WRITE_MULTIPLE    		0xC5 	//������д
#define  IDE_CMD_SET_MULTIPLE     		0xC6	//
#define  IDE_CMD_READ_DMA          		0xC8	//DMA��
#define  IDE_CMD_WRITE_DMA              0xCA	//DMAд
#define  IDE_CMD_GET_MEDIA_STATUS       0xDA	//ȡ�豸״̬
#define  IDE_CMD_STANDBY_IMMEDIATE 		0xE0 	//Standby Immediate����
#define  IDE_CMD_IDLE_IMMEDIATE  		0xE1 	//Idle Immediate����
#define  IDE_CMD_SETFEATURES 			0xEF	//
#define  IDE_CMD_IDENTIFY               0xEC	//
#define  IDE_CMD_MEDIA_EJECT            0xED	//����

/*״̬�Ĵ������λ*/
#define ATA_STAT_REG_ERR  0x01 /*�豸�����˴���*/
#define ATA_STAT_REG_IDX   0x02   /**/
#define ATA_STAT_REG_CORR   0x04   /**/
#define ATA_STAT_REG_DRQ   0x08  /*�����ݴ�������*/
#define ATA_STAT_REG_DSC   0x10  /**/
#define ATA_STAT_REG_DF    0x20  /**/
#define ATA_STAT_REG_DRDY   0x40  /*�豸׼����*/
#define ATA_STAT_REG_BSY   0x80  /*�豸æ*/
#define ATA_STAT_BSY_DRDY (ATA_STAT_REG_BSY+ATA_STAT_REG_DRDY)
#define ATA_STAT_BSY_DRQ (ATA_STAT_REG_BSY+ATA_STAT_REG_DRQ)


#define HD_DATA     0x1f0       //Ӳ�����ݼĴ���
#define HD_ERROR    0x1f1       //����Ĵ���
#define HD_NSECTOR  0x1f2       //�����Ĵ���
#define HD_SECTOR   0x1f3       //�����żĴ���
#define HD_LCYL     0x1f4       //����żĴ��������ֽ�
#define HD_HCYL     0x1f5       //����żĴ��������ֽ�
#define HD_CURRENT  0x1f6       //������/��ͷ�Ĵ�����dΪ��������hΪ��ͷ
#define HD_STATUS   0x1f7       //״̬�Ĵ���
#define HD_FEATURE  HD_ERROR    //��Ϊ��״̬ʱΪ����Ĵ�����д״̬ʱΪдǰ�����Ĵ���
#define HD_PRECOMP  HD_FEATURE 
#define HD_COMMAND  HD_STATUS   //��״̬ʱΪ��״̬�Ĵ�����д״̬ʱΪ����Ĵ���
#define HD_CMD      0x3f6       //Ӳ�̿��ƼĴ���
#define HD_ALTSTATUS    0x3f6       //����״̬�Ĵ�����������ж�λ
//Ӳ��״̬�Ĵ��������б�
#define ERR_STAT        0x01    //����ִ�д���
#define INDEX_STAT      0x02 //�յ�����
#define ECC_STAT        0x04    //ECCУ�����
#define DRQ_STAT        0x08 //�����������
#define SEEK_STAT       0x10 //������Ѱ������
#define SERVICE_STAT        SEEK_STAT
#define WRERR_STAT      0x20 //����������
#define READY_STAT      0x40 //������׼������
#define BUSY_STAT       0x80 //������æµ
//Ӳ�̴���Ĵ�������
#define MARK_ERR        0x01    //���ݱ�־��ʧ
#define TRK0_ERR        0x02    //�ŵ�0����
#define ABRT_ERR        0x04    //�������
#define MCR_ERR         0x08    //����ı�
#define ID_ERR          0x10    //IDδ�ҵ�
#define MC_ERR          0x20    //ý��ı�
#define ECC_ERR         0x40    //ECC����
#define BBD_ERR         0x80    //������
#define ICRC_ERR        0x80    //CRC����

/*�豸��ͷ�Ĵ���*/
#define ATA_DevReg_DEV0   0x00 /*�豸Ϊ����*/
#define ATA_DevReg_DEV1   0x10 /*�豸Ϊ����*/
#define ATA_DevReg_b5    0x20 /*����λ��Ϊ1*/
#define ATA_DevReg_LBA   0x40 /*ģʽѡ��λ*/
#define ATA_DevReg_b7    0x80 /*����λ��Ϊ1*/

/*���ƼĴ���*/
#define  CF_SOFT_RESET 0x04 /*�����λ��Ч*/
#define  CF_DISABLE_INT 0x00 /*�����ж�*/
#define  CF_ENABLE_INT 0x02 /*ʹ���ж�*/


/*���������16λ�����������,��8λ��ATA����Ĵ���һ��,��8λ�Զ���*/
#define ATA_EER_REG_AMNF 0x01 /*һ�����*/
#define ATA_EER_REG_ABRT  0x04 /*ָ����Ч����*/
#define ATA_EER_REG_IDNF  0x10 /*Ѱ������ַ����*/
#define ATA_EER_REG_UNC  0x40 /*�����˲��ɾ����Ĵ���*/
#define ATA_EER_REG_BBK 0x80 /*���ִ����*/
/*�Զ���*/
#define CARD_IS_FALSE 0x00ff /*CF�������ڻ���Ч*/
#define CARD_IS_TRUE 0x0000 /*CF����Ч*/
#define CARD_TIMEOUT 0x0100 /*��CF��������ʱ����*/
#define THIS_DEVICE ((ATA_DevReg&0x10)>>4) /*��ǰ�������豸��ATA_SelectDevice����ѡ����������豸��ͨ���ò�������ȷ���ղ�ѡ�����豸*/

���� ATAPI ָ����ܣ�
IDENTIFYDEVICE READSECTOR READMULTIPLE READDMA WRITESECTOR WRITEMULTIPLE WRITEDMA SETFEATURES 
0xec 			0x20 		0x 			0xc8 		0x30 		0xc5 		0xca 	0xef 

��Ҫ:CHSѰַ�Ǵ�1������ʼ��LBAѰַ�Ǵ�0������ʼ.ʵ�ʱ��ʱһ��Ҫע����һ�㡣


�˿�                  LBA							CHS
 
					��			д					��			д
1f0h			���ݼĴ���
1f1h			����Ĵ���	�����Ĵ���				����Ĵ���	дǰԤ�����Ĵ���
1f2h			�������Ĵ���
1f3h			LBA���ַ(0~7λ)					�����żĴ���
1f4h			LBA���ַ(8~15λ)					����żĴ���(�ߵ�ַ)
1f5h			LBA���ַ(16~23λ)					����żĴ���(�͵�ַ)
1f6h			��������+LBA���ַ(24~27λ)			��������/��ͷ��
1f7h			״̬�Ĵ���		����Ĵ���			״̬�Ĵ���	����Ĵ���
36f			����״̬�Ĵ���		Ӳ�̿��ƼĴ���		��			Ӳ�̿��ƼĴ���

