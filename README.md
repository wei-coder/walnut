# walnut
一个自制的小OS内核，主要是为了重新熟悉操作系统原理。

刚刚启动，还在努力添加代码中。

已完成：
  bootloader：借用了GRUB，后续会自行实现一个bootloader。
  内存管理：   实现了内存分页，以及简单的内存申请，释放功能。
  进程管理：   实现了时间片轮转调度的进程调度。
  中断管理：   搭建了基本的中断处理框架，借用linux的代码，实现了基本的硬件中断的处理。
下一步继续实现文件系统，并准备把内核搬到硬盘。
