#include <stdio.h>
#include <unistd.h>         // 包含系统调用的头文件，如getpid等
#include <string.h>         // 包含字符串处理函数的头文件，如strlen等

int main()
{
    printf("Pid: %d\n", getpid());

    // 打开文件temp.txt，模式为"w"，表示以写方式打开文件
    // 如果文件不存在，则创建该文件
    // 如果文件已存在，则覆盖原有内容
    FILE* fp = fopen("temp.txt", "w");

    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    const char* message = "abcd";       // 定义一个字符串常量message，字符串常量是只读的，不能被修改

    // 将字符串message写入文件
    // fwrite函数用于将数据写入文件
    // 第一个参数是要写入的数据的指针
    // 第二个参数是每个数据单元的大小，这里是1字节（即每个字符）
    // 第三个参数是数据单元的数量，这里是字符串的长度加1（包括字符串结束符'\0'）
    // 第四个参数是文件指针
    fwrite(message, strlen(message) + 1, 1, fp);

    fclose(fp);                         // 关闭文件

    return 0;
}
