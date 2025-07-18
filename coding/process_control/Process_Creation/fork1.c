#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define N 5
int main()
{
    printf("pid:%d before!\n", getpid());  // fork 调用前，打印一次
    fork();                                // 创建子进程
    printf("pid:%d after!\n", getpid());   // 父子进程都会执行这一句

    return 0;
}
