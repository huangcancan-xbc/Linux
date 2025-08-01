#include "comm.hpp"

int main()
{
    Init fifo;                                          // 初始化管道
    int fd = open(FIFO_FILE, O_RDONLY);                 // 以只读方式打开管道
    if(fd < 0)
    {
        perror("open");
        exit(FIFO_OPEN_ERR);
    }

    while(true)
    {
        char buf[1024] = {0};
        int x = read(fd, buf, sizeof(buf));             // 读取管道数据，x 为读取的字节数
        if(x > 0)
        {
            buf[x] = 0;                                 // 将完整的字节流转换为字符串，并添加结束符‘\0’
            cout << "客户端说:" << buf << endl;
        }
    }

    close(fd);                                          // 关闭管道

    return 0;
}