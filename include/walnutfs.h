/*
filename:	walnutfs.h
author:	  	wei-coder
time:		2018-09-15
purpose:	walnutfs文件系统的函数声明。
*/

typedef struct wfs_sbinfo
{
	u32	s_dblocks;			//文件系统中使用的blocks的总数
	u32	s_sectsize;			//底层磁盘一个扇区的大小
	u32	s_agblocks;			//一个AG包含地blocks数（AG 分配组 alloc group）
	u32	s_agcount;			//整个文件系统的AG数
	u32	s_inodesize;		//每个inode的大小
	u32	s_inopblocks;		//每个block中inode的个数
	u32	s_logstart;			//存放journal的第一个block
	u32	s_logblocks;		//存放log的总blocks数
	u32	s_icount;			//整个文件系统已经分配的inode个数
	u32	s_ifree;			//整个文件系统空闲的inode个数
	u32	s_versionnum;		//文件系统的版本号
	u32	s_feature;			//同上
}wfs_sbinfo_t;

typedef struct wfs_ag_free
{
	u32	agf_magicnum;
	u32	agf_versionnum;
	u32	agf_seqno;			//AG的num
	u32	agf_length;			//包含的blocks个数
	u32	agf_roots[2];		//两颗B+树的树根
	u32	agf_spare0;			//空闲inode？/blocks？
	u32	agf_levels[2];		//两颗B+树的深度
	u32	agf_spare1;			//
	u32	agf_flfirst;		//AG freelist的第一个block
	u32	agf_fllast;			//AG freelist的最后一个block
	u32	agf_flcount;		//AG freelist中blocks的个数
	u32	agf_freeblks;		//当前可用的blocks个数
	u32	agf_longest;		//AG中最长连续空间的block个数
	u32	agf_btreeblks;		//B+树描述的blocks个数
}wfs_agf_t;

typedef struct wfs_ag_inode
{
	u32	agi_magicnum;
	u32	agi_versionnum;
	u32	agi_seqno;			//AG的num
	u32	agi_length;			//AG的size
	u32	agi_count;			//AG中已分配的inode个数
	u32	agi_roots;			//AG inode B+树的root节点的block num
	u32	agi_spare0;			//空闲inode？/blocks？
	u32	agi_levels[2];		//两颗B+树的深度
	u32	agi_spare1;			//
	u32	agi_flfirst;		//AG freelist的第一个block
	u32	agf_fllast;			//AG freelist的最后一个block
	u32	agf_flcount;		//AG freelist中blocks的个数
	u32	agf_freeblks;		//当前可用的blocks个数
	u32	agf_longest;		//AG中最长连续空间的block个数
	u32	agf_btreeblks;		//B+树描述的blocks个数
}wfs_agi_t;

typedef struct wfs_ag_free_list
{

}wfs_agfl_t;


typedef struct wfs_fileblk
{
	struct wfs_fileblk * next;
	int		blksize;
	char	data[];
}wfs_fblk_t;

struct wfs_file_data
{
	char	fname[DNAME_LEN_MAX];
	u32		filelen;
	wfs_fblk_t * fblk_h;
};

int read_super_wf(sb_t * pSblk);
void init_walnutfs();
dentry_t * wfs_mount(struct file_system_type * fs_type, int flags, const char * fs_name, void * data);

