#include <unistd.h>  // 提供 getpid(), getppid()
#include <stdio.h>


int main()
{
    while (1)
    {
        // 获取当前进程 ID
        pid_t pid = getpid();
        printf("当前进程 ID (PID): %d\n", pid);

        // 获取父进程 ID
        pid_t ppid = getppid();
        printf("父进程 ID (PPID): %d\n", ppid);

        sleep(1);  // 睡眠 1 秒
    }

    return 0;
}
