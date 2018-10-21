虚拟文件系统

vfs 四大对象

Super_Block: 超级块，该对象代表了一个具体文件系统。磁盘中的超级块，用于存储一个具体文件系统的基本信息。
					内存中的Super_block，则是一个包含了一个具体文件系统的数据以及相关操作函数的对象。
					超级块操作函数包含了:
					对本文件系统的索引节点操作的相关函数，如alloc_inode，dirty_inode，wirte_inode，drop_inode等
					对文件系统本身进行操作的函数:sync_fs，freeze_fs，unfreeze_fs，statfs，remount_fs等等
					

inode: 索引节点，该对象代表了一个具体的文件或者目录。存放文件系统中各种文件（文件，目录，软连接，设备）的元数据。每个文件/目录都对应一个inode。
				存放在内存中的inode，包含了，文件类型、权限、拥有者、大小、时间戳，块指针等相关信息。
				而存放在磁盘上的inode，同样包含如上信息。
				inode对象的成员函数包含了:
				create 新建索引节点inode
				lookup 根据指定的文件名，查找索引节点
				link、unlink 创建或去除连接，所谓硬连接，是指存在多个dentry，指向同一个inode
				mkdir、rmdir 创建或删除目录，目录也是文件，相当于创建一个inode。
				mknod 创建特殊文件(设备，命名管道，套接字)
				rename 修改文件名
				setattr、getattr、setxattr，getxattr，listxattr，removeattr，对文件属性的修改。

dentry:目录项对象，该对象也代表了一个具体的文件或目录，主要是描述文件的逻辑属性或者说目录结构。
		成员函数包括:
		d_reavlidate 该函数判断目录对象是否有效。使一个dentry重新生效
		d_hash    该目录生成散列值，当目录项要加入到散列表时，VFS要调用此函数。
		d_compare 该函数来比较name1和name2这两个文件名。使用该函数要加dcache_lock锁。
		d_delete  当d_count=0时，VFS调用次函数。使用该函数要叫 dcache_lock锁。
		d_release 当该目录对象将要被释放时，VFS调用该函数。
		d_iput    用于一个dentry释放它的inode（d_count不等于零）

file: 文件对象，