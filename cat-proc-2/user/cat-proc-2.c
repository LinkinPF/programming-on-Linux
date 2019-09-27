# include <stdio.h> 
# include <unistd.h>
# include <signal.h>
# include <sys/time.h>
# include <string.h>
# include <stdlib.h>
# include <pthread.h>
# include <ctype.h>
# include "dirent.h"

# include "variable.h"
//# include "fill_arff.c"

# include <time.h>


//字符串倒置
//好玩
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

// 将字符数组 s 颠倒
void reverse(char *s,int length ) {
    int mid = length / 2;
    for (int i = 0; i < mid; i++) {
        char t = s[i];
        s[i] = s[length - i-1];
        s[length - i -1] = t;
    }
}

// 计算整数 n 的位数
int count(int n)
{
    int t = 0;
    do {
        n /= 10;  
        t++;
    } while (n > 0);
    return t;
}

//整数转字符串
char* itoaa(int n)   // 1234 
{
    int num =count(n);
    char * chs = (char *)malloc(sizeof(char)*(num+1));
    int i = 0,t;
    do {
        t= n % 10;
        chs[i++] = t+'0';
        n /= 10;
    } while (n > 0);
    reverse(chs, num);// 逆转，将 4321 逆转成 1234
    chs[num] = '\0';
    return chs;
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
//对应类型1，写入内存信息
extern int fill_arff_mes_mm(FILE *out)
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

    if(fputs(buffer, out) == EOF) {
        printf("out文件写入非数据部分失败!\n");
        ret = 0;
    }

    return ret;
}

//向arff文件写入非数据部分
//对应类型2，写入stat信息
extern int fill_arff_mes_stat(FILE *out)
{
    int ret = 1;
    //需要往文件中写入的属性信息，需要自己手动写
    unsigned char *buffer = "@relation 'Test'\n\n\
@attribute 'pid'  real\n\
@attribute 'comm'   { HeapSort,MergeSort,QuickSort }\n\
@attribute 'task_state'  {    }}\n\
@attribute 'ppid'  real\n\
@attribute 'pgid'  real\n\
@attribute 'sid'  real\n\
@attribute 'tty_nr'  real\n\
@attribute 'tty_pgrp'  real\n\
@attribute 'task_flags'  real\n\
@attribute 'min_flt'  real\n\
@attribute 'cmin_flt'  real\n\
@attribute 'maj_flt'  real\n\
@attribute 'cmaj_flt'  real\n\
@attribute 'utime'  real\n\
@attribute 'stime'   real\n\
@attribute 'cutime'  real\n\
@attribute 'cstime'  real\n\
@attribute 'priority'  real\n\
@attribute 'nice'  real\n\
@attribute 'num_threads'  real\n\
@attribute 'it_real_value'   real\n\
@attribute 'start_time'  real\n\
@attribute 'vsize'   real\n\
@attribute 'rss'  real\n\
@attribute 'rlim'  real\n\
@attribute 'start_code'  real\n\
@attribute 'end_code'  real\n\
@attribute 'start_stack'  real\n\
@attribute 'kstkesp'   real\n\
@attribute 'kstkeip'   real\n\
@attribute 'pendingsig'  real\n\
@attribute 'block_sig'   real\n\
@attribute 'sigign'  real\n\
@attribute 'sigcatch'  real\n\
@attribute 'wchan'  real\n\
@attribute 'nswap'  real\n\
@attribute 'cnswap'  real\n\
@attribute 'exit_signal'   real\n\
@attribute 'task_cpu'  real\n\
@attribute 'task_rt_priority'   real\n\
@attribute 'task_policy'   real\n\
@data\n";

    if(fputs(buffer, out) == EOF) {
        printf("out文件写入非数据部分失败!\n");
        ret = 0;
    }

    return ret;
}




//对应读取内存信息
void * write_weka(void * pthread)
{
    // clock_t start,end;
    // double total;
    // start = clock();

    struct pthread_data * data = pthread;
    char proc_path[LEN];
    char user_path[LEN];
    FILE *in,*out;  
    unsigned char buffer[512] = {'\0'};             //读proc文件存放的缓冲区

    if(1 == type) {
        sprintf(proc_path,"%s%s","/proc/weka/",data->name);
        //user_path = data->name;
    } else if(2 == type) {
        sprintf(proc_path,"%s%s%s","/proc/",data->name,"/stat");
        sprintf(user_path,"%s%s",data->name,".arff");
    }
    //printf("name:%s  proc_path:%s   count:%d\n",data->name, proc_path, data->count);

    if((data->count)-- >= 1)
    {
        in = open_file(in,proc_path,"r");
        out = open_file(out,user_path,"a");

        if(fgets(buffer,512,in) == NULL) {          //从fp中读取一行,存在buffer中,读取的最大字符数是255,包括包括空字符\0 
            printf("in文件读入失败!\n");  //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
            exit(1);
        }
        //第二种是直接读取proc本身就有的文件，需要将空格替换成,
        char s[512]={'\0'};
        strcpy(s,buffer);
        for(int i=0;i<strlen(s);i++) {
            if(s[i]=='\0')
                break;
            if(s[i]==' ') {
                s[i]=',';
            }
        }
        if(fputs(s,out) == EOF) {              //调用fputs函数写入文件，不管成功或失败都会返回一条信息。
            printf("out文件写入数据部分失败!\n");
            exit(1);
        }
        //fputs("\n",out);
        fclose(in); 
        fclose(out);
    } else {
        return NULL;
    }

    // end = clock();
    // total=(double)(end-start)/CLOCKS_PER_SEC;
    // printf("%f\n",total);

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
        printf("共提取%d个进程内核空间数据\n\n",num);
        printf("提取已完成!\n");
        exit(0);
    }

    for(int i=0; i<file_num; i++) {
        if(thread[i].data.count>=0) {
            pthread_create(&(thread[i].t),NULL,write_weka,(void *)&((thread[i].data)));
            //printf("call_on_weka:%dthread:%s\n",i,thread[i].data.name);
            pthread_join(thread[i].t,NULL);
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

    if(1 == type ) {
        ret = fill_arff_mes_mm(out); 
    } else if(2 == type) {
        ret = fill_arff_mes_stat(out);
    }
      
    fclose(out); 
    return ret;
}




void create_wekafile_all()
{
    //初始化一些变量 
    int i = 0;
    int filesize = 0;  
    DIR *dir = NULL;  
    char  *path; 
    //char angry[LEN];
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

            //根据数据类型做不同的处理，使用局部变量path的
            if(1 == type) {
                path = entry->d_name;
                arff_name[i] = path;
            } else if(2 == type) {
                arff_name[i] = itoaa(atoi(entry->d_name));

                path = entry->d_name;
                sprintf(path,"%s%s",path,".arff");
                //第二种情况没用arff_name[i]
                //arff_name[i] = path;

                //printf("arff_name:%s\n",arff_name[i]);
                //printf("arff_name:%s\n",arff_name[i]);
            }
            i++;
            //printf("create_wekafile_all  i:%d\n",i);

            if(!create_wekafile( path )) {
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
    if(argc != 4) {
        printf("Usage: %s <提取数据类型：1-内存，2-stat> <间隔微妙数> <提取数据的条数>\n",argv[0]);
        exit(1);
    }

    //设置信号处理函数
    signal(SIGALRM,call_on_weka);

    //保存数据类型
    type = atoi(argv[1]);

    //初始化要提取的数据数量
    num = atoi(argv[3]);

    create_wekafile_all();

    //初始化线程数据
    for(int i=0; i<file_num; i++) {
            (thread[i].data).name = arff_name[i];
            (thread[i].data).count = num;
    }

    //初始化定时器
    if(set_ticker(atoi(argv[2])) == -1) {
         perror("初始化定时器失败\n");
    } else {
        //开始提取数据
        printf("成功! 开始提取数据\n\n");
        printf("当前每隔 %f s提取一次数据\n\n",atoi(argv[2])/1000000.0);
        //printf("按回车结束数据提取\n\n");
        while(1) {
            pause();
        }
    }
    
    return 0;    
}  



