#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("原进程即将被替换！\n");
    execl("/bin/ls", "ls", "-l", NULL);     // 执行 ls -l 命令
    printf("这里不会被执行！你看不到我！\n");
    perror("execl failed");                 // 若替换失败才会执行到这里
    return 1;
}
