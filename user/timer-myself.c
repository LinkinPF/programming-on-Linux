#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
# include <stdlib.h>
//#include <time.h>

//存放想要生成的.arff文件名和路径名,加上.arff不超过20个字符
# define LEN 200
char name[LEN];
//提取数据的次数
unsigned long num = 0;


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
void fill_arff_mes(FILE *out)
{
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
        exit(1);
    }
}
 

void write_weka(int sig)
{
    if(sig != SIGALRM)  
        return;

    FILE *in,*out;  
    unsigned char buffer[255] = {'\0'};             //读proc文件存放的缓冲区

    //printf("write_weka中的path:%s\n",path);
    
    in = open_file(in,"/proc/zcy/weka.arff","r");
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
    num++;

    fclose(in); 
    fclose(out); 
    return;
}


int main(int argc,char *argv[])
{
    int res= 0;
    FILE *in,*out;                                  //指向读取和写入文件的指针
    unsigned char buffer[255] = {'\0'};             //读proc文件存放的缓冲区
    struct itimerval tick;

    //检测命令行参数个数是否正确
    if(argc != 3) {
        printf("Usage: %s <数据集的文件名> <间隔微妙数>\n",argv[0]);
        exit(1);
    }



    signal(SIGALRM,write_weka);
    memset(&tick,0,sizeof(tick));
    int usec = atoi(argv[2]);           //每隔多少微妙提取一次数据
    //第一次执行function延迟的秒数,sec单位是秒,usec单位是微秒,但是微妙赋值不能大于100 0000
    tick.it_value.tv_sec= 0;       
    tick.it_value.tv_usec= 1;
    //以后延迟多长时间执行function,sec单位是秒,usec单位是微秒,但是微妙赋值不能大于100 0000
    tick.it_interval.tv_sec= 0;    
    tick.it_interval.tv_usec= usec;

    //可以用来检测读写消耗的时间
    //clock_t start_t, end_t;
    //double total_t;

    //拼接字符串以便形成.arff文件
    strcpy(name,argv[1]);
    strcat(name,".arff");
    out = open_file(out,name,"w");
    in = open_file(in,"/proc/zcy/weka.arff","r");
    //写入arff文件前面的信息
    fill_arff_mes(out);  
    //关闭打开的文件         
    fclose(in); 
    fclose(out); 

    //初始化定时器
    res = setitimer(ITIMER_REAL,&tick,NULL);
    if(res)
    {
         printf("Settimer failed!!\n");
    }

    //开始提取数据
    printf("成功! 开始提取数据\n\n");
    printf("当前每隔 %f s提取一次数据\n\n",usec/1000000.0);
    printf("按回车结束数据提取\n");
    while(1)
    {
        if(getchar())
            break;
    }
    printf("共提取 %ld 条数据\n\n",num);
    printf("拜!\n");
    return 0;
}

