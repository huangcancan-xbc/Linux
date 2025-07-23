#include <fcntl.h>                      // 包含文件控制相关的定义和函数声明
#include <unistd.h>                     // 包含 POSIX 操作函数的声明，如open、close、write等
#include <stdio.h>                      // 包含标准输入输出函数的声明，如perror等

int main()
{
    // 打开或创建文件，并设置为只写模式、创建文件、截断文件
    int fd = open("demo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open failed");          // 打印错误信息，open failed 为自定义的错误前缀
        return 1;                       // 打开文件失败，返回非零值表示程序异常退出
    }

    const char* msg = "实习 / 工作要掌握 open 的实战用法！\n";       // 定义要写入文件的字符串，将字符串写入文件
    
    write(fd, msg, strlen(msg));        // 第一个参数是文件描述符，第二个是数据的指针，第三个是数据的长度
    
    close(fd);                          // 关闭文件，释放资源

    return 0;                           // 程序正常退出
}
