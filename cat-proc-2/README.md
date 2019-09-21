# 快速使用

## 内核模块的使用(kernel)：
sudo insmod XXX.ko num=<监控的进程数量> pid=XXX,XXX,XXX,XXX    
注意：确保输入的pid号的个数和监控的进程数量一致，目前提供10个以内的监控(人为限制)，如果想增加，可以修改代码，过程很简单     
   
## 用户空间使用(user)：
编译：gcc cat-proc-2.c variable.h -lpthread     
执行：./<可执行文件>  有说明

祝你开心！

