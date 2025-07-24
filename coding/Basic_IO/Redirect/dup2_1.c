#include <stdio.h>              // printf, perror
#include <unistd.h>             // dup2, close
#include <fcntl.h>              // open

int main()
{
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);        // 打开一个文件，准备写入（如果不存在就会自动创建）
    if (fd < 0)
    {
        perror("open failed");
        return 1;
    }

    close(1);                                                               // 关闭标准输出 1（stdout）

    dup2(fd, 1);                                                            // 将打开的文件描述符复制到 1（标准输出）

    printf("这行文字被写入到 output.txt 文件中啦！\n");                        // 现在所有的 printf 都不会打印到屏幕，而是写到 output.txt
    dprintf(fd, "这是直接用 fd 写的一行文字！\n");                             // 可以继续用 fd 写数据（可选）

    close(fd);                                                              // 关闭文件
    return 0;
}
