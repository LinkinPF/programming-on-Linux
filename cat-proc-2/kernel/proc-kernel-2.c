# include <linux/kernel.h>
# include <linux/module.h>
# include <linux/init.h>
# include <linux/slab.h>
# include <linux/proc_fs.h>
# include <linux/seq_file.h>
# include <linux/list.h>
# include <linux/init_task.h>
# include <linux/sched.h>
//# include <linux/internal.h>

//内存信息
# include <linux/hugetlb.h>
# include <linux/sched/mm.h>
# include <linux/hugetlb.h>

 char * name;

//要监控进程的个数,目前不超过十个
static int num;
module_param(num,int,0644);
//MODULE_PARM_DESC(g_intnum,"数组元素个数");

static int pid[10] = {0};
module_param_array(pid,int,&num,S_IRUGO);


void show_weka_mem(struct seq_file *m, struct mm_struct *mm)
{
	unsigned long text, lib, swap, anon, file, shmem;
	unsigned long hiwater_vm, total_vm, hiwater_rss, total_rss;

	anon = get_mm_counter(mm, MM_ANONPAGES);
	file = get_mm_counter(mm, MM_FILEPAGES);
	shmem = get_mm_counter(mm, MM_SHMEMPAGES);

	hiwater_vm = total_vm = mm->total_vm;
	if (hiwater_vm < mm->hiwater_vm)
		hiwater_vm = mm->hiwater_vm;
	hiwater_rss = total_rss = anon + file + shmem;
	if (hiwater_rss < mm->hiwater_rss)
		hiwater_rss = mm->hiwater_rss;

	text = (PAGE_ALIGN(mm->end_code) - (mm->start_code & PAGE_MASK)) >> 10;
	lib = (mm->exec_vm << (PAGE_SHIFT-10)) - text;
	swap = get_mm_counter(mm, MM_SWAPENTS);
	//写入想要打印的信息，注意锁,各种格式
	seq_printf(m,
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu,"
		"%lu\n",
		hiwater_vm << (PAGE_SHIFT-10),
		total_vm << (PAGE_SHIFT-10),
		mm->locked_vm << (PAGE_SHIFT-10),
		mm->pinned_vm << (PAGE_SHIFT-10),
		hiwater_rss << (PAGE_SHIFT-10),
		total_rss << (PAGE_SHIFT-10),
		anon << (PAGE_SHIFT-10),
		file << (PAGE_SHIFT-10),
		shmem << (PAGE_SHIFT-10),
		mm->data_vm << (PAGE_SHIFT-10),
		mm->stack_vm << (PAGE_SHIFT-10), text, lib,
		mm_pgtables_bytes(mm) >> 10,
		swap << (PAGE_SHIFT-10));
}

static int weka_show(struct seq_file *m, void *v)
{
	struct task_struct *p;
	struct mm_struct *mm;

	//让p指向想要找的进程描述符
    p = &init_task;
    for_each_process(p)
        if(p->pid == *(pid_t*)(PDE_DATA(m->file->f_inode)))
        	break;

	//接下来开始寻找并打印内存方面的数据
    mm = get_task_mm(p);
	if (mm) {
		show_weka_mem(m, mm);
		mmput(mm);
	}

	return 0;
}

static int weka_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, weka_show, NULL);
}


const struct file_operations weka_fops = {
	.open           = weka_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	//.write          = weka_write,
	.release        = single_release,
};


static int __init param_init(void)
{
    int ret = -ENOMEM;
    int i = 0;
	struct proc_dir_entry *pe;

 	name = (char *)kmalloc(strlen("zcy/") + 8,GFP_ATOMIC);

	proc_mkdir("zcy", NULL);
	for(i=0; i<num; i++) {
		sprintf(name,"%s%d%s","zcy/",pid[i],".arff");


		if(!(pe = proc_create_data(name,S_IFREG | 0644, NULL, &weka_fops, &pid[i])))
			goto err_proc;
	}

	printk("weka-module loaded.\n");

	return 0;

err_proc:
	return ret;

    return 0;
}


static void __exit param_exit(void)
{
	int i;
	for(i=0; i<num; i++) {
		sprintf(name,"%s%d","zcy/",pid[i]);
		remove_proc_entry(name, NULL);
	}
	remove_proc_entry("zcy", NULL);
	return;
}


MODULE_LICENSE("GPL");
module_init(param_init);
module_exit(param_exit);

