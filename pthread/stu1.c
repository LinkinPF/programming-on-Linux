# include <unistd.h>
# include <stdio.h>
# include <pthread.h>

# define NUM 5

void main(void)
{
	//定义两个线程
	pthread_t t1,t2;

	void * print_msg(void *);

	//开始运行线程
	//第一个参数:线程的地址
	//第二个参数:指向线程属性的指针,NULL是默认属性
	//第三个参数:要执行的函数名称
	//第四个参数:要传递给函数的参数的指针,使用hello来执行print_msg
	pthread_create(&t1,NULL,print_msg,(void *)"hello");
	pthread_create(&t2,NULL,print_msg,(void *)"world\n");

	//等待线程执行完返回
	//第一个参数:要等待的线程
	//第二个参数:一个指向返回值的指针
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
}

void * print_msg(void *m)
{
	char * cp = (char *) m;
	int i;
	for(i=0; i<NUM; i++)
	{
		printf("%s",m);
		fflush(stdout);
		sleep(1);
	}
	return NULL;
}





