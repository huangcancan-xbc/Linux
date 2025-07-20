#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        exit(66); // 子进程退出码设为 66
    }
    else
    {
        int status = 0;
        waitpid(pid, &status, 0);

        printf("原始 status：%d (0x%x)\n", status, status);

        if (WIFEXITED(status))
        {
            printf("正常退出，返回值 = %d\n", WEXITSTATUS(status));
            printf("手动解析返回值 = %d\n", (status >> 8) & 0xFF);
        }
        else
        {
            printf("非正常退出\n");
        }
    }
    return 0;
}
