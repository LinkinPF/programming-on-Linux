#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
# include <stdlib.h>
//#include <time.h>

//存放想要生成的.arff文件名和路径名,加上.arff不超过20个字符
# define LEN 20
char name[LEN];
//提取数据的次数
unsigned long num;


//打开或者创建一个文件
FILE *open_file(FILE *file,char name[],char *mode)
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

    printf("fill_arff_mes:%d\n",ret);

    if(fputs(buffer, out) == EOF)  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
    {
        printf("out文件写入非数据部分失败!\n");
        ret = 0;
    }

    printf("fill_arff_mes:%d\n",ret);
    return ret;
}
 

void write_weka(int sig)
{
    if(sig != SIGALRM)  
        return;

    FILE *in,*out;  
    unsigned char buffer[255] = {'\0'};             //读proc文件存放的缓冲区
    
    in = open_file(in,"/proc/zcy/weka","r");
    out = open_file(out,name,"a");

    if(fgets(buffer,255,in) == NULL)   //从fp中读取一行,存在buffer中,读取的最大字符数是255,包括包括空字符\0 
    {
        printf("in文件读入失败!\n");  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
        exit(1);
    }
    if(fputs(buffer,out) == EOF)  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
    {
        printf("out文件写入数据部分失败!\n");
        exit(1);
    }
    fclose(in); 
    fclose(out);
    if(num-- == 1)
    {
        fclose(in); 
        fclose(out);
        printf("提取完成\n\n拜!\n");
        exit(0);
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
int create_wekafile(char *s)
{
    int ret = 0;
    FILE *out;                                  //指向读取和写入文件的指针
    strcpy(name,s);
    strcat(name,".arff");
    out = open_file(out,name,"w");
    ret = fill_arff_mes(out);   
    fclose(out); 
    return ret;
}



//可以用来检测读写消耗的时间
//clock_t start_t, end_t;
//double total_t;
int main(int argc,char *argv[])
{
    
    //检测命令行参数个数是否正确
    if(argc != 4) {
        printf("Usage: %s <数据集的文件名> <间隔微妙数> <提取数据的条数>\n",argv[0]);
        exit(1);
    }
    num = atoi(argv[3]);
    //设置函数
    signal(SIGALRM,write_weka);
    //忽略ctrl+c
    signal(SIGINT,SIG_IGN);

    //初始化文件
    if(!create_wekafile(argv[1]))
    {
        perror("创建arff文件失败\n");
        exit(1);
    }
    
    //初始化定时器
    if(set_ticker(atoi(argv[2])) == -1)
    {
         perror("初始化定时器失败\n");
    }
    else
    {
        //开始提取数据
        printf("成功! 开始提取数据\n\n");
        printf("当前每隔 %f s提取一次数据\n\n",atoi(argv[2])/1000000.0);
        printf("按回车结束数据提取\n\n");
        while(1)
        {
            if(getchar())
                break;
            pause();
        }
    }

    return 0;
}

