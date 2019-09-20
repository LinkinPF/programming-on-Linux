# include <stdio.h> 
# include <unistd.h>
# include <signal.h>
# include <sys/time.h>
# include <string.h>
# include <stdlib.h>
# include <pthread.h>
# include <ctype.h>

#include "dirent.h"

#define  FilePath "/proc/zcy"

//存放想要生成的.arff文件名和路径名,加上.arff不超过20个字符
# define LEN 20
char name[LEN];

//文件个数/进程个数
int file_num = 0;

//存放要读取的文件路径
//char * proc_file[10];

//存放生成的文件名
char * dir_name[10];

//提取数据的次数
unsigned long num = 0;

//创建线程时用来传入的参数
struct pthread_data {
    //保存文件名
    char * name;
    //未提取的数据数量
    int count;
};

//一个线程
struct pthread {
    pthread_t t;
    struct pthread_data data;
};
//定义10个线程
struct pthread thread[10];


//字符串倒置
char * strrev(char *str)
{
   char *right = str;
   char *left = str;
   char ch;
   while (*right)   right++;
   right--;
   while (left < right)
   {
       ch = *left;
        *left++ = *right;
        *right-- = ch;
   }

   return(str);
}



//打开或者创建一个文件
FILE *open_file(FILE *file,char *name,char *mode)
{
    if((file = fopen(name,mode)) == NULL )
    {
        printf("Can't open %s\n",name);
        exit(1);       
    }
    return file;
}


//向arff文件写入非数据部分
int fill_arff_mes(FILE *out)
{
    int ret = 1;
    //需要往文件中写入的属性信息，需要自己手动写
    unsigned char *buffer = "@relation 'KDDTest'\n\n\
@attribute 'VmPeak'  real\n\
@attribute 'VmSize'   real\n\
@attribute 'VmLck'  real\n\
@attribute 'VmPin'  real\n\
@attribute 'VmHWM'  real\n\
@attribute 'VmRSS'  real\n\
@attribute 'RssAnon'  real\n\
@attribute 'RssFile'  real\n\
@attribute 'RssShmem'  real\n\
@attribute 'VmData'  real\n\
@attribute 'VmStk'  real\n\
@attribute 'VmExe'  real\n\
@attribute 'VmLib'  real\n\
@attribute 'VmPTE'  real\n\
@attribute 'VmSwap'   real\n\n\
@data\n";

    if(fputs(buffer, out) == EOF)  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
    {
        printf("out文件写入非数据部分失败!\n");
        ret = 0;
    }

    return ret;
}

void * write_weka(void * pthread)
{
    struct pthread_data * data = pthread;
    char path[LEN];
    FILE *in,*out;  
    unsigned char buffer[255] = {'\0'};             //读proc文件存放的缓冲区

    sprintf(path,"%s%s","/proc/zcy/",data->name);
    //printf("name:%s  path:%s   count:%d\n",data->name, path, data->count);

    if((data->count)-- >= 1)
    {
        in = open_file(in,path,"r");
        out = fopen((char *)(data->name), "a");
        //out = open_file(out,(char *)(data->name),"a");

        if(fgets(buffer,255,in) == NULL) {          //从fp中读取一行,存在buffer中,读取的最大字符数是255,包括包括空字符\0 
            printf("in文件读入失败!\n");  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
            exit(1);
        }

        if(fputs(buffer,out) == EOF) {              //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
            printf("out文件写入数据部分失败!\n");
            exit(1);
        }
        fclose(in); 
        fclose(out);
    } else {
        return NULL;
    }

    return NULL;
}



void call_on_weka(int sig)
{
    if(sig != SIGALRM)  
        return;
    int num=0;

    for(int i=0; i<file_num; i++) {
        if(thread[i].data.count<0) {
            num++;
        }
    }
    if(num==file_num) {
        printf("提取已完成!\n");
        exit(0);
    }

    for(int i=0; i<file_num; i++) {

        if(thread[i].data.count>=0) {
            pthread_create(&(thread[i].t),NULL,write_weka,(void *)&((thread[i].data)));
            pthread_join(thread[i].t,NULL);
        } else {
            printf("%s已提取完成!\n",thread[i].data.name);
        }
    }

    return;
}



//初始化定时器
int set_ticker(int user_usecs)
{
    struct itimerval tick;
    long n_sec,n_usecs; 

    n_sec = user_usecs/1000000;
    n_usecs = user_usecs % 1000000L;

    //第一次执行function延迟的秒数,sec单位是秒,usec单位是微秒,但是微妙赋值不能大于100 0000
    tick.it_value.tv_sec= 1;       
    tick.it_value.tv_usec= 0;
    //以后延迟多长时间执行function,sec单位是秒,usec单位是微秒,但是微妙赋值不能大于100 0000
    tick.it_interval.tv_sec= n_sec;    
    tick.it_interval.tv_usec= n_usecs;

    return setitimer(ITIMER_REAL,&tick,NULL);
}


//初始化文件
int create_wekafile(char * file)
{
    int ret = 0;
    FILE *out;                                  //指向读取和写入文件的指针
    
    out = open_file(out,file,"w");
    ret = fill_arff_mes(out);   
    fclose(out); 

    return ret;
}




void create_wekafile_all()
{
    //初始化一些变量 
    int i = 0;
    int filesize = 0;  
    DIR *dir = NULL;  
    struct dirent *entry;

    //开始创建文件
    if((dir = opendir(FilePath))==NULL)  {  
        printf("opendir failed!");  
        exit(1);  
    } else {
        while(entry = readdir(dir)) {  
            //不读取.和..文件
            if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            dir_name[i] = entry->d_name;
            i++;

            if(!create_wekafile( entry->d_name )) {
                perror("创建arff文件失败\n");
                exit(1);
            }
            // printf("filename%d = %s",i,entry->d_name);  //输出文件或者目录的名称
            // printf("filetype = %d\n",entry->d_type);  //输出文件类型   
        }
        file_num = i;
        closedir(dir); 
    }

    return;
}



int main(int argc,char *argv[])  
{     

    //检测命令行参数个数是否正确
    if(argc != 3) {
        printf("Usage: %s <间隔微妙数> <提取数据的条数>\n",argv[0]);
        exit(1);
    }

    int i;

    //设置信号处理函数
    signal(SIGALRM,call_on_weka);

    //初始化数据数量
    num = atoi(argv[2]);

    create_wekafile_all();

    for(i=0; i<file_num; i++) {
        (thread[i].data).name = dir_name[i];
        (thread[i].data).count = num;
    }

    //初始化定时器
    if(set_ticker(atoi(argv[1])) == -1)
    {
         perror("初始化定时器失败\n");
    }
    else
    {
        //开始提取数据
        printf("成功! 开始提取数据\n\n");
        printf("当前每隔 %f s提取一次数据\n\n",atoi(argv[1])/1000000.0);
        printf("按回车结束数据提取\n\n");
        while(1)
        {
            pause();
        }
    }
    
    return 0;    
}  



