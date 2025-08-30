#include <unistd.h>
#include <cstdlib>
#include <iostream>

int main()
{
    // 创建守护进程
    if (daemon(0, 0) == -1)
    {
        perror("daemon");
        exit(1);
    }

    // 守护进程的主循环
    while (true)
    {
        // 写日志或者处理任务
        sleep(1);
    }
}