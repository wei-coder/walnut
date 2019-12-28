/*
filename:	walnutfs.h
author:	  	wei-coder
time:		2018-09-15
purpose:	walnutfs�ļ�ϵͳ�ĺ���������
*/

typedef struct wfs_sbinfo
{
	u32	s_dblocks;			//�ļ�ϵͳ��ʹ�õ�blocks������
	u32	s_sectsize;			//�ײ����һ�������Ĵ�С
	u32	s_agblocks;			//һ��AG������blocks����AG ������ alloc group��
	u32	s_agcount;			//�����ļ�ϵͳ��AG��
	u32	s_inodesize;		//ÿ��inode�Ĵ�С
	u32	s_inopblocks;		//ÿ��block��inode�ĸ���
	u32	s_logstart;			//���journal�ĵ�һ��block
	u32	s_logblocks;		//���log����blocks��
	u32	s_icount;			//�����ļ�ϵͳ�Ѿ������inode����
	u32	s_ifree;			//�����ļ�ϵͳ���е�inode����
	u32	s_versionnum;		//�ļ�ϵͳ�İ汾��
	u32	s_feature;			//ͬ��
}wfs_sbinfo_t;

typedef struct wfs_ag_free
{
	u32	agf_magicnum;
	u32	agf_versionnum;
	u32	agf_seqno;			//AG��num
	u32	agf_length;			//������blocks����
	u32	agf_roots[2];		//����B+��������
	u32	agf_spare0;			//����inode��/blocks��
	u32	agf_levels[2];		//����B+�������
	u32	agf_spare1;			//
	u32	agf_flfirst;		//AG freelist�ĵ�һ��block
	u32	agf_fllast;			//AG freelist�����һ��block
	u32	agf_flcount;		//AG freelist��blocks�ĸ���
	u32	agf_freeblks;		//��ǰ���õ�blocks����
	u32	agf_longest;		//AG��������ռ��block����
	u32	agf_btreeblks;		//B+��������blocks����
}wfs_agf_t;

typedef struct wfs_ag_inode
{
	u32	agi_magicnum;
	u32	agi_versionnum;
	u32	agi_seqno;			//AG��num
	u32	agi_length;			//AG��size
	u32	agi_count;			//AG���ѷ����inode����
	u32	agi_roots;			//AG inode B+����root�ڵ��block num
	u32	agi_spare0;			//����inode��/blocks��
	u32	agi_levels[2];		//����B+�������
	u32	agi_spare1;			//
	u32	agi_flfirst;		//AG freelist�ĵ�һ��block
	u32	agf_fllast;			//AG freelist�����һ��block
	u32	agf_flcount;		//AG freelist��blocks�ĸ���
	u32	agf_freeblks;		//��ǰ���õ�blocks����
	u32	agf_longest;		//AG��������ռ��block����
	u32	agf_btreeblks;		//B+��������blocks����
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

