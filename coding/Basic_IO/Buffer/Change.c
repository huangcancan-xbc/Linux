#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    printf("打印到屏幕（终端）前先睡5秒...\n");
    sleep(5);  // 打印了才 sleep，说明是“行缓冲”
    return 0;
}
