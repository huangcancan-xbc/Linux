#include <stdio.h>
#include <unistd.h>
int main()
{
    chdir("/home/hcc");						// 改变当前工作目录为/home/hcc(注意：目录不存在，可能会导致后续操作失败)
    printf("Pid: %d\n", getpid());          // 打印进程ID

    FILE* fp = fopen("temp.txt", "w");      // 打开文件，如果文件不存在则创建，如果存在则覆盖
    if (fp == NULL)
    {
        perror(" fopen");		            // 如果打开文件失败，打印错误信息
        return 1;
    }

    fclose(fp);				                // 关闭文件
    sleep(100);			                    // 休眠100秒

    return 0;
}
