参考:
http://www.verysource.com/code/9693198_1/IDE.h.html
https://max.book118.com/html/2014/1013/9848872.shtm
https://blog.csdn.net/u011164819/article/details/52043678
http://blog.chinaunix.net/uid-20235103-id-1970841.html
https://wenda.chinawjzx.com/ztnews/idfdce5dbcjh657ij8gide89.html

hd寄存器:

COMMAND: 读时状态，写时命令
	7---------6---------5---------4------------3------------2---------------1---------------0
	BSY		DRDY	  DF/SE       #           DRQ			#				ERR
	bit7: 0:空闲 1:忙
	bit6  0:没有准备好 1 准备好


DEVICE: 
	7--------6---------5---------4------------3------------2---------------1---------------0
	1	    L/C		   1		DEV
	bit 6 0:chs  1:LAB
	bit 4 0:master 1:slave

SEC_COUNT:
	读取的扇区数

LBA_LOW/MID/HIGH:
	0,0,0 LBA:0
	CHS: 0磁头，0磁道，0扇区

循环读取bit7，6,如果 为01，则命令执行完毕。
此时如果bit0 为1则执行出错，否则成功。如果出错则读出 HD_PRI_ERR_FEAT.

错误寄存器:

位意义
0  AMNF，没找到所要访问的扇区的数据区。
1 TK0NF，在执行恢复RECALIBRATE命令时，0磁道没有发现。
2 ABRT，对硬盘发非法指令或因硬盘驱动器故障而造成命令执行的中断。
3 MAC，该信号用来向主机发出通知，表示介质的改变。
4 IDNF，没有找到访问的扇区，或CRC发生错误。
5 MC，这是发送给主机一个信号以通知主机使用新的传输介质。
6 UNC，在读扇区命令时出现不能校正的ECC错误，因此此次数据传输无效。
7  BBK，在访问扇区的ID数据场发现坏的数据块时会置1

扇区计数寄存器：指明所要读写的扇区总数，其中0表示传输256个扇区，如果在数据读写过程发生错误，寄存器将保存尚未读写的扇区数目。  

磁道数寄存器：指明所要读写的磁道数。 

驱动器磁头寄存器：指定硬盘驱动器号与磁头号和寻址方式，如表4所示。
7 6 5  4   3    2     1   0 
1 L 1 DRV  HS3  HS2  HS1 HS0  

表5  IDE状态寄存器位意义
0  ERR，错误(ERROR)，该位为1表示在结束前次的命令执行时发生了无法恢复的错误。在错误寄存器中保存了更多的错误信息。
1 IDX，反映从驱动器读入的索引信号。
2 CORR，该位为1时，表示已按ECC算法校正硬盘的读数据。
3 DRQ，为1表示请求主机进行数据传输(读或写)。
4 DSC，为1表示磁头完成寻道操作，已停留在该道上。
5 DF，为1时，表示驱动器发生写故障。
6 DRDY，为1时表示驱动器准备好，可以接受命令。
7  BSY，为1时表示驱动器忙(BSY)，正在执行命令。在发送命令前先判断该位。

命令寄存器：包含执行的命令代码。当向命令寄存器写命令时，相关该命令的参数必须先写入。在写命令时，状态寄存器的BSY位置1。如果命令是非法，则中止执行。
在ATA标准中，IDE命令一共有30多个，其中有10个是通用型(也称强制型)命令。主要的参数如表6所示，表中的Word指2个字节。

Word 1 		Word 3 		Word 6  			Word 10-19 			Word 60-61  

磁道数   	磁头数	每磁道的扇区数		20个ASCII码系列号	LBA可以寻找的最大扇区数

20H  读扇区命令(带重试)：从硬盘读取指定磁道、磁头上的1~256个扇区到主机。送到主机的数据可以添加4个字节的ECC校验码，
读的起始扇区号和扇区个数在命令块指定。这条命令也隐藏着寻找指定的磁道号，所以不需要另外的寻道命令。

30H  写扇区命令(带重试)：本命令是将主机内的数据写入硬盘，可以写指定磁道、磁头上的1~256个扇区，与读扇区命令相似，
这条命令也隐藏着寻找指定的磁道号，写的起始扇区号和扇区个数由命令块指定。

90H  硬盘诊断命令：以判断硬盘是否已经连接到主机上，可以读取错误寄存器以检查需要的结果，如果是01H或81H表示设备是好的，
否则表示设备没有连接或设备是坏的。

设备控制寄存器：将该寄存器的SRST位设置为1，可以使硬盘驱动器处于复位状态。IEN表示是否允许中断，其中0为允许。由此可见，
对该寄存器发送0X0CH命令即令硬盘复位，其格式如表7所示。

  表7  IDE   设备控制寄存器

7 6 5 4 3 	2 	1 		0 
- - - - 1 SRST  IEN  	0  


  LBA=(柱面号*磁头数+磁头号)*扇区数+扇区编号

  PIO方式读命令的执行过程如下： 
1)  根据要读的扇区位置，向控制寄存器1F2H~1F6H发命令参数，等驱动器的状态寄存器1F7H的DRDY置位后进入下一步； 
2)  主机向驱动器命令控制器1F7H发送读命令20H； 
3)  驱动器设置状态寄存器1F7H中的BSY位，并把数据发送到硬盘缓冲区； 
4)  驱动器读取一个扇区后，自动设置状态寄存器1F7H的DRQ数据请求位，并清除BSY位忙信号。 DRQ位通知主机现在可以从缓
	冲区中读取512字节或更多(如果用READ LONG COMMAND命令)的数据，同时向主机发INTRQ中断请求信号； 
5)  主机响应中断请求，开始读取状态寄存器1F7H，以判断读命令执行的情况，同时驱动器清除INTRQ中断请求信号； 
6)  根据状态寄存器，如果读取的数据命令执行正常，进入7)，如果有错误，进入错误处理，如果是ECC错误，再读取一次，否则退出程序运行； 
7)  主机通过数据寄存器1F0H读取硬盘缓冲区中的数据到主机缓冲区中，当一个扇区数据被读完，扇区计数器-1，如果扇区计数器不为0，进入3)，否则进入8)； 
8)  当所有的请求扇区的数据被读取后，命令执行结束。 

PIO方式写命令的执行过程如下： 
1)  根据要写的扇区位置，向驱动器控制寄存器1F2H~1F6H发出命令参数，等驱动器DRDY置位后进入下一步； 
2)  主机向驱动器命令控制器1F7H发送写命令30H； 
3)  驱动器在状态寄存器1F7H中设置DRQ数据请求信号； 
4)  主机通过数据寄存器1F0H把指定内存(BUF)中的数据传输到缓冲区； 
5)  当缓冲区满，或主机送完512个字节的数据后，驱动器设置状态寄存器1F7H中的BSY位，并清除DRQ数据请求信号； 
6)  缓冲区中的数据开始被写入驱动器的指定的扇区中，一旦处理完一个扇区，驱动器马上清除BSY信号，同时设置INTRQ； 
7)  主机读取驱动器的状态1F7H和错误寄存器1F1H，以判断写命令执行的情况，如果有无法克服的错误(如坏盘)，退出本程序，否则，进入下一步； 
8)  如果还有扇区进行写操作，进入3)否则，进入下一步；   
9)  当所有的请求扇区的数据被写后，命令执行结束。 
	虽然硬盘缓冲区可以容纳很多个扇区，但每读/写一个扇区，就判断其命令执行的状态寄存器，这样就可以保证读写的数据的正确性。 


*ATA 指令代码*/ 
#define  IDE_CMD_ATAPI_RESET       		0x08	//重置reset命令
#define  IDE_CMD_RECALIBRATE       		0x10	//重新校准
#define  IDE_CMD_READ              		0x20 	//读扇区命令
#define  IDE_CMD_WRITE             		0x30 	//写扇区命令
#define  IDE_CMD_VERIFY            		0x40	//验证
										0x50	//format
										0x60	//init
#define  IDE_CMD_SEEK              		0x70	//设置读写点
#define	 IDE_CMD_EXEC_DIAGNOSE     		0x90	//硬盘诊断
#define  IDE_CMD_SET_DRIVE_PARAMETERS  	0x91	//设定硬盘参数
#define  IDE_CMD_READ_MULTIPLE     		0xC4 	//多扇区读
#define  IDE_CMD_WRITE_MULTIPLE    		0xC5 	//多扇区写
#define  IDE_CMD_SET_MULTIPLE     		0xC6	//
#define  IDE_CMD_READ_DMA          		0xC8	//DMA读
#define  IDE_CMD_WRITE_DMA              0xCA	//DMA写
#define  IDE_CMD_GET_MEDIA_STATUS       0xDA	//取设备状态
#define  IDE_CMD_STANDBY_IMMEDIATE 		0xE0 	//Standby Immediate待命
#define  IDE_CMD_IDLE_IMMEDIATE  		0xE1 	//Idle Immediate空闲
#define  IDE_CMD_SETFEATURES 			0xEF	//
#define  IDE_CMD_IDENTIFY               0xEC	//
#define  IDE_CMD_MEDIA_EJECT            0xED	//弹出

/*状态寄存器相关位*/
#define ATA_STAT_REG_ERR  0x01 /*设备发生了错误*/
#define ATA_STAT_REG_IDX   0x02   /**/
#define ATA_STAT_REG_CORR   0x04   /**/
#define ATA_STAT_REG_DRQ   0x08  /*有数据传输请求*/
#define ATA_STAT_REG_DSC   0x10  /**/
#define ATA_STAT_REG_DF    0x20  /**/
#define ATA_STAT_REG_DRDY   0x40  /*设备准备好*/
#define ATA_STAT_REG_BSY   0x80  /*设备忙*/
#define ATA_STAT_BSY_DRDY (ATA_STAT_REG_BSY+ATA_STAT_REG_DRDY)
#define ATA_STAT_BSY_DRQ (ATA_STAT_REG_BSY+ATA_STAT_REG_DRQ)


#define HD_DATA     0x1f0       //硬盘数据寄存器
#define HD_ERROR    0x1f1       //错误寄存器
#define HD_NSECTOR  0x1f2       //扇区寄存器
#define HD_SECTOR   0x1f3       //扇区号寄存器
#define HD_LCYL     0x1f4       //柱面号寄存器，低字节
#define HD_HCYL     0x1f5       //柱面号寄存器，高字节
#define HD_CURRENT  0x1f6       //驱动器/磁头寄存器，d为驱动器，h为磁头
#define HD_STATUS   0x1f7       //状态寄存器
#define HD_FEATURE  HD_ERROR    //当为读状态时为错误寄存器，写状态时为写前补偿寄存器
#define HD_PRECOMP  HD_FEATURE 
#define HD_COMMAND  HD_STATUS   //读状态时为主状态寄存器，写状态时为命令寄存器
#define HD_CMD      0x3f6       //硬盘控制寄存器
#define HD_ALTSTATUS    0x3f6       //类似状态寄存器，但清除中断位
//硬盘状态寄存器错误列表
#define ERR_STAT        0x01    //命令执行错误
#define INDEX_STAT      0x02 //收到索引
#define ECC_STAT        0x04    //ECC校验错误
#define DRQ_STAT        0x08 //数据请求服务
#define SEEK_STAT       0x10 //驱动器寻道结束
#define SERVICE_STAT        SEEK_STAT
#define WRERR_STAT      0x20 //驱动器故障
#define READY_STAT      0x40 //驱动器准备就绪
#define BUSY_STAT       0x80 //驱动器忙碌
//硬盘错误寄存器错误
#define MARK_ERR        0x01    //数据标志丢失
#define TRK0_ERR        0x02    //磁道0出错
#define ABRT_ERR        0x04    //命令放弃
#define MCR_ERR         0x08    //请求改变
#define ID_ERR          0x10    //ID未找到
#define MC_ERR          0x20    //媒体改变
#define ECC_ERR         0x40    //ECC错误
#define BBD_ERR         0x80    //坏扇区
#define ICRC_ERR        0x80    //CRC错误

/*设备磁头寄存器*/
#define ATA_DevReg_DEV0   0x00 /*设备为主盘*/
#define ATA_DevReg_DEV1   0x10 /*设备为从盘*/
#define ATA_DevReg_b5    0x20 /*保留位，为1*/
#define ATA_DevReg_LBA   0x40 /*模式选择位*/
#define ATA_DevReg_b7    0x80 /*保留位，为1*/

/*控制寄存器*/
#define  CF_SOFT_RESET 0x04 /*软件复位有效*/
#define  CF_DISABLE_INT 0x00 /*禁能中断*/
#define  CF_ENABLE_INT 0x02 /*使能中断*/


/*错误代码由16位二进制数组成,低8位与ATA错误寄存器一致,高8位自定义*/
#define ATA_EER_REG_AMNF 0x01 /*一般错误*/
#define ATA_EER_REG_ABRT  0x04 /*指令无效出错*/
#define ATA_EER_REG_IDNF  0x10 /*寻扇区地址出错*/
#define ATA_EER_REG_UNC  0x40 /*发生了不可纠正的错误*/
#define ATA_EER_REG_BBK 0x80 /*发现错误块*/
/*自定义*/
#define CARD_IS_FALSE 0x00ff /*CF卡不存在或无效*/
#define CARD_IS_TRUE 0x0000 /*CF卡有效*/
#define CARD_TIMEOUT 0x0100 /*对CF卡操作超时出错*/
#define THIS_DEVICE ((ATA_DevReg&0x10)>>4) /*当前操作的设备，ATA_SelectDevice函数选择了主或从设备，通过该操作可以确定刚才选定的设备*/

常用 ATAPI 指令介绍：
IDENTIFYDEVICE READSECTOR READMULTIPLE READDMA WRITESECTOR WRITEMULTIPLE WRITEDMA SETFEATURES 
0xec 			0x20 		0x 			0xc8 		0x30 		0xc5 		0xca 	0xef 

主要:CHS寻址是从1扇区开始，LBA寻址是从0扇区开始.实际编程时一定要注意这一点。


端口                  LBA							CHS
 
					读			写					读			写
1f0h			数据寄存器
1f1h			错误寄存器	特征寄存器				错误寄存器	写前预补偿寄存器
1f2h			扇区数寄存器
1f3h			LBA块地址(0~7位)					扇区号寄存器
1f4h			LBA块地址(8~15位)					柱面号寄存器(高地址)
1f5h			LBA块地址(16~23位)					柱面号寄存器(低地址)
1f6h			驱动器号+LBA块地址(24~27位)			驱动器号/磁头号
1f7h			状态寄存器		命令寄存器			状态寄存器	命令寄存器
36f			交换状态寄存器		硬盘控制寄存器		无			硬盘控制寄存器

