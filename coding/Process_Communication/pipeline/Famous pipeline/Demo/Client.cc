#include "comm.hpp"

int main()
{
    int fd = open(FIFO_FILE, O_WRONLY);             // 以只写方式打开管道文件
    if(fd < 0)
    {
        perror("open");
        exit(FIFO_OPEN_ERR);
    }

    string str;                                     // 定义消息字符串
    while(true)
    {
        cout << "请输入要发送的消息：";
        getline(cin, str);                          // 读取用户输入的消息（一整行）

        write(fd, str.c_str(), str.size());         // 向管道文件写入消息，str.c_str()是 string 转换为 C 风格字符串 的方法
    }
    
    close(fd);                                      // 关闭管道文件
    return 0;
}