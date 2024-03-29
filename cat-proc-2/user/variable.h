//定义变量
# include <pthread.h>
# include <time.h>

#define  FilePath "/proc/weka"

//存放想要生成的.arff文件名和路径名,加上.arff不超过20个字符
# define LEN 20
char name[LEN];

//提取的数据类型
int type = 0;

//文件个数/线程个数
int file_num = 0;

//存放pid
char * pid_name[10];

//存放生成的文件名
char * arff_name[10];

//提取数据的次数
unsigned long num = 0;

//创建线程时用来传入的参数
struct pthread_data {
    //保存文件名
    char * name;
    //未提取的数据数量
    int count;
    void * data;
};

//一个线程
struct pthread {
    pthread_t t;
    struct pthread_data data;
};

//定义10个线程
struct pthread thread[10];
