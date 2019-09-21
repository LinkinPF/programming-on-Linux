//这个程序是多线程使用main中定义结构体来避免使用互斥量和全局变量

# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <ctype.h>

struct arg_set {
	char * fname;
	int count;
};

void main(int ac, char *av[])
{
	void *count_words(void *);

	pthread_t t1,t2;
	struct arg_set args1,args2;

	if( ac != 3 ) {
		printf("usage:%s file1 file 2\n",av[0]);
		exit(1);
	}

	args1.fname = av[1];
	args1.count = 0;
	pthread_create(&t1,NULL,count_words,(void *)&args1);

	args2.fname = av[2];
	args2.count = 0;
	pthread_create(&t2,NULL,count_words,(void *)&args2);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	printf("%5d:%s\n",args1.count,av[1]);
	printf("%5d:%s\n",args2.count,av[2]);
	printf("%5d:total_words\n",args1.count+args2.count);

}

void * count_words(void * a)
{
	struct arg_set * args = a;
	
	//代码实体


	printf("hello\n");

}


