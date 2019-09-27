# include <linux/kernel.h>
# include <linux/module.h>
# include <linux/init.h>
# include <linux/slab.h>
# include <linux/proc_fs.h>
# include <linux/seq_file.h>
# include <linux/list.h>
# include <linux/init_task.h>
# include <linux/sched.h>
# include <linux/kallsyms.h>
//# include "internal.h"

//内存信息
# include <linux/hugetlb.h>
# include <linux/sched/mm.h>
# include <linux/hugetlb.h>

//调度信息
# include <linux/pid.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/mman.h>
#include <linux/sched/mm.h>
#include <linux/sched/numa_balancing.h>
#include <linux/sched/task_stack.h>
#include <linux/sched/task.h>
#include <linux/sched/cputime.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/smp.h>
#include <linux/signal.h>
#include <linux/highmem.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/times.h>
#include <linux/cpuset.h>
#include <linux/rcupdate.h>
#include <linux/delayacct.h>
#include <linux/seq_file.h>
#include <linux/pid_namespace.h>
#include <linux/ptrace.h>
#include <linux/tracehook.h>
#include <linux/string_helpers.h>
#include <linux/user_namespace.h>
#include <linux/fs_struct.h>

#include <asm/pgtable.h>
#include <asm/processor.h>
#include <linux/jiffies.h>
#include <linux/sched/signal.h>



#define KSTK_ESP(task)		(task_pt_regs(task)->sp)

//# include <func.c>

char * name;


//要提取的数据类型
//1-->内存信息
//2-->stat信息
static int type;
module_param(type,int,0644);
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


// static int do_task_stat(struct seq_file *m, struct pid_namespace *ns,
// 			struct pid *pid, struct task_struct *task, int whole)
// {	
// 	// //使用内核函数
// 	// char* (*my_get_task_state)(struct task_struct *) = (void*)kallsyms_lookup_name("get_task_state");
// 	void (*my_collect_sigign_sigcatch)(struct task_struct *,sigset_t *,sigset_t *) = (void*)kallsyms_lookup_name("collect_sigign_sigcatch");
// 	// unsigned long (*my_get_wchan)(struct task_struct *) = (void*)kallsyms_lookup_name("get_wchan");
// 	void (*my_put_task_stack)(struct task_struct *) = (void*)kallsyms_lookup_name("put_task_stack");
// 	// __u64 (*my_delayacct_blkio_ticks)(struct task_struct *) = (void*)kallsyms_lookup_name("delayacct_blkio_ticks");
// 	// u64 (*my_nsec_to_clock_t)(u64) = (void*)kallsyms_lookup_name("nsec_to_clock_t");
// 	void (*my_thread_group_cputime_adjusted)(struct task_struct *,u64 *,u64 *) = (void*)kallsyms_lookup_name("thread_group_cputime_adjusted");
// 	struct sighand_struct * (*my_lock_task_sighand)(struct task_struct *,unsigned long *) = (void*)kallsyms_lookup_name("lock_task_sighand");
// 	bool (*my_ptrace_may_access)(struct task_struct *,unsigned int) = (void*)kallsyms_lookup_name("ptrace_may_access");
	
// 	//bool ptrace_may_access(struct task_struct *task, unsigned int mode);
// 	//struct sighand_struct *lock_task_sighand(struct task_struct *tsk,unsigned long *flags)
// 	//void thread_group_cputime_adjusted(struct task_struct *p, u64 *ut, u64 *st)
// 	//u64 nsec_to_clock_t(u64 x)
// 	//__u64 delayacct_blkio_ticks(struct task_struct *tsk)
// 	//void put_task_stack(struct task_struct *tsk)
// 	//unsigned long get_wchan(struct task_struct *p);
// 	//collect_sigign_sigcatch(struct task_struct *p, sigset_t *ign,sigset_t *catch)

// 	unsigned long vsize, eip, esp, wchan = 0;
// 	int priority, nice;
// 	int tty_pgrp = -1, tty_nr = 0;
// 	sigset_t sigign, sigcatch;
// 	char state;
// 	pid_t ppid = 0, pgid = -1, sid = -1;
// 	int num_threads = 0;
// 	int permitted;
// 	struct mm_struct *mm;
// 	unsigned long long start_time;
// 	unsigned long cmin_flt = 0, cmaj_flt = 0;
// 	unsigned long  min_flt = 0,  maj_flt = 0;
// 	u64 cutime, cstime, utime, stime;
// 	u64 cgtime, gtime;
// 	unsigned long rsslim = 0;
// 	char tcomm[sizeof(task->comm)];
// 	unsigned long flags;

// 	//state = *my_get_task_state(task);
// 	vsize = eip = esp = 0;
// 	permitted = my_ptrace_may_access(task, PTRACE_MODE_READ_FSCREDS | PTRACE_MODE_NOAUDIT);
// 	mm = get_task_mm(task);
// 	if (mm) {
// 		vsize = PAGE_SIZE * mm->total_vm;
// 		/*
// 		 * esp and eip are intentionally zeroed out.  There is no
// 		 * non-racy way to read them without freezing the task.
// 		 * Programs that need reliable values can use ptrace(2).
// 		 *
// 		 * The only exception is if the task is core dumping because
// 		 * a program is not able to use ptrace(2) in that case. It is
// 		 * safe because the task has stopped executing permanently.
// 		 */
// 		if (permitted && (task->flags & PF_DUMPCORE)) {
// 			if (try_get_task_stack(task)) {
// 				eip = KSTK_EIP(task);
// 				esp = KSTK_ESP(task);
// 				my_put_task_stack(task);
// 			}
// 		}
// 	}

// 	get_task_comm(tcomm, task);

// 	sigemptyset(&sigign);
// 	sigemptyset(&sigcatch);
// 	cutime = cstime = utime = stime = 0;
// 	cgtime = gtime = 0;

// 	if (my_lock_task_sighand(task, &flags)) {
// 		struct signal_struct *sig = task->signal;

// 		if (sig->tty) {
// 			struct pid *pgrp = tty_get_pgrp(sig->tty);
// 			tty_pgrp = pid_nr_ns(pgrp, ns);
// 			put_pid(pgrp);
// 			tty_nr = new_encode_dev(tty_devnum(sig->tty));
// 		}

// 		num_threads = get_nr_threads(task);
// 		my_collect_sigign_sigcatch(task, &sigign, &sigcatch);

// 		cmin_flt = sig->cmin_flt;
// 		cmaj_flt = sig->cmaj_flt;
// 		cutime = sig->cutime;
// 		cstime = sig->cstime;
// 		cgtime = sig->cgtime;
// 		rsslim = READ_ONCE(sig->rlim[RLIMIT_RSS].rlim_cur);

// 		/* add up live thread stats at the group level */
// 		if (whole) {
// 			struct task_struct *t = task;
// 			do {
// 				min_flt += t->min_flt;
// 				maj_flt += t->maj_flt;
// 				gtime += task_gtime(t);
// 			} while_each_thread(task, t);

// 			min_flt += sig->min_flt;
// 			maj_flt += sig->maj_flt;
// 			my_thread_group_cputime_adjusted(task, &utime, &stime);
// 			gtime += sig->gtime;
// 		}

// 		sid = task_session_nr_ns(task, ns);
// 		ppid = task_tgid_nr_ns(task->real_parent, ns);
// 		pgid = task_pgrp_nr_ns(task, ns);

// 		unlock_task_sighand(task, &flags);
// 	}

// 	// if (permitted && (!whole || num_threads < 2))
// 	// 	wchan = my_get_wchan(task);
// 	// if (!whole) {
// 	// 	min_flt = task->min_flt;
// 	// 	maj_flt = task->maj_flt;
// 	// 	task_cputime_adjusted(task, &utime, &stime);
// 	// 	gtime = task_gtime(task);
// 	// }

// 	// /* scale priority and nice values from timeslices to -20..20 */
// 	// /* to make it look like a "normal" Unix priority/nice value  */
// 	// priority = task->prio - MAX_RT_PRIO;
// 	// nice = task_nice(task);

// 	// /* convert nsec -> ticks */
// 	// start_time = my_nsec_to_clock_t(task->real_start_time);

// 	// seq_printf(m, "%d (%s) %c", pid_nr_ns(pid, ns), tcomm, state);
// 	// seq_put_decimal_ll(m, " ", ppid);
// 	// seq_put_decimal_ll(m, " ", pgid);
// 	// seq_put_decimal_ll(m, " ", sid);
// 	// seq_put_decimal_ll(m, " ", tty_nr);
// 	// seq_put_decimal_ll(m, " ", tty_pgrp);
// 	// seq_put_decimal_ull(m, " ", task->flags);
// 	// seq_put_decimal_ull(m, " ", min_flt);
// 	// seq_put_decimal_ull(m, " ", cmin_flt);
// 	// seq_put_decimal_ull(m, " ", maj_flt);
// 	// seq_put_decimal_ull(m, " ", cmaj_flt);
// 	// seq_put_decimal_ull(m, " ", my_nsec_to_clock_t(utime));
// 	// seq_put_decimal_ull(m, " ", my_nsec_to_clock_t(stime));
// 	// seq_put_decimal_ll(m, " ", my_nsec_to_clock_t(cutime));
// 	// seq_put_decimal_ll(m, " ", my_nsec_to_clock_t(cstime));
// 	// seq_put_decimal_ll(m, " ", priority);
// 	// seq_put_decimal_ll(m, " ", nice);
// 	// seq_put_decimal_ll(m, " ", num_threads);
// 	// seq_put_decimal_ull(m, " ", 0);
// 	// seq_put_decimal_ull(m, " ", start_time);
// 	// seq_put_decimal_ull(m, " ", vsize);
// 	// seq_put_decimal_ull(m, " ", mm ? get_mm_rss(mm) : 0);
// 	// seq_put_decimal_ull(m, " ", rsslim);
// 	// seq_put_decimal_ull(m, " ", mm ? (permitted ? mm->start_code : 1) : 0);
// 	// seq_put_decimal_ull(m, " ", mm ? (permitted ? mm->end_code : 1) : 0);
// 	// seq_put_decimal_ull(m, " ", (permitted && mm) ? mm->start_stack : 0);
// 	// seq_put_decimal_ull(m, " ", esp);
// 	// seq_put_decimal_ull(m, " ", eip);
// 	// /* The signal information here is obsolete.
// 	//  * It must be decimal for Linux 2.0 compatibility.
// 	//  * Use /proc/#/status for real-time signals.
// 	//  */
// 	// seq_put_decimal_ull(m, " ", task->pending.signal.sig[0] & 0x7fffffffUL);
// 	// seq_put_decimal_ull(m, " ", task->blocked.sig[0] & 0x7fffffffUL);
// 	// seq_put_decimal_ull(m, " ", sigign.sig[0] & 0x7fffffffUL);
// 	// seq_put_decimal_ull(m, " ", sigcatch.sig[0] & 0x7fffffffUL);

// 	// /*
// 	//  * We used to output the absolute kernel address, but that's an
// 	//  * information leak - so instead we show a 0/1 flag here, to signal
// 	//  * to user-space whether there's a wchan field in /proc/PID/wchan.
// 	//  *
// 	//  * This works with older implementations of procps as well.
// 	//  */
// 	// if (wchan)
// 	// 	seq_puts(m, " 1");
// 	// else
// 	// 	seq_puts(m, " 0");

// 	// seq_put_decimal_ull(m, " ", 0);
// 	// seq_put_decimal_ull(m, " ", 0);
// 	// seq_put_decimal_ll(m, " ", task->exit_signal);
// 	// seq_put_decimal_ll(m, " ", task_cpu(task));
// 	// seq_put_decimal_ull(m, " ", task->rt_priority);
// 	// seq_put_decimal_ull(m, " ", task->policy);
// 	// seq_put_decimal_ull(m, " ", my_delayacct_blkio_ticks(task));
// 	// seq_put_decimal_ull(m, " ", my_nsec_to_clock_t(gtime));
// 	// seq_put_decimal_ll(m, " ", my_nsec_to_clock_t(cgtime));

// 	// if (mm && permitted) {
// 	// 	seq_put_decimal_ull(m, " ", mm->start_data);
// 	// 	seq_put_decimal_ull(m, " ", mm->end_data);
// 	// 	seq_put_decimal_ull(m, " ", mm->start_brk);
// 	// 	seq_put_decimal_ull(m, " ", mm->arg_start);
// 	// 	seq_put_decimal_ull(m, " ", mm->arg_end);
// 	// 	seq_put_decimal_ull(m, " ", mm->env_start);
// 	// 	seq_put_decimal_ull(m, " ", mm->env_end);
// 	// } else
// 	// 	seq_puts(m, " 0 0 0 0 0 0 0");

// 	// if (permitted)
// 	// 	seq_put_decimal_ll(m, " ", task->exit_code);
// 	// else
// 	// 	seq_puts(m, " 0");

// 	// seq_putc(m, '\n');
// 	// if (mm)
// 	// 	mmput(mm);
// 	return 0;
// }



static int weka_show(struct seq_file *m, void *v)
{
	struct task_struct *p;
	//struct pid *task;
	//struct pid_namespace *ns;
	struct mm_struct *mm;

	//让p指向想要找的进程描述符
    p = &init_task;
    for_each_process(p)
        if(p->pid == *(pid_t*)(PDE_DATA(m->file->f_inode)))
        	break;
    
    //显示进程的调度信息

	
    // task = get_task_pid(p,PIDTYPE_PID);
    // ns = ns_of_pid(task);
    // return do_task_stat(m,ns,task,p,1);


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

 	name = (char *)kmalloc(strlen("weka/") + 12,GFP_ATOMIC);

	proc_mkdir("weka", NULL);


	if(type == 1)  { 		//１代表提取内存信息
	
		for(i=0; i<num; i++) {
			sprintf(name,"%s%d%s","weka/",pid[i],".arff");
			if(!(pe = proc_create_data(name,S_IFREG | 0644, NULL, &weka_fops, &pid[i])))
				goto err_proc;
		}

	} else if(type == 2) {	//2代表提取stat信息
		for(i=0; i<num; i++) {
			sprintf(name,"%s%d","weka/",pid[i]);
			if(!(pe = proc_create(name,S_IFREG | 0644, NULL, &weka_fops)))
				goto err_proc;
		}
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

	if(1 == type) {
		for(i=0; i<num; i++) {
			sprintf(name,"%s%d%s","weka/",pid[i],".arff");
			remove_proc_entry(name, NULL);
		}

	} else if (2 == type) {
		for(i=0; i<num; i++) {
			sprintf(name,"%s%d","weka/",pid[i]);
			remove_proc_entry(name, NULL);
		}

	}


	remove_proc_entry("weka", NULL);
	return;
}


MODULE_LICENSE("GPL");
module_init(param_init);
module_exit(param_exit);

