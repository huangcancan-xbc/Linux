// 测试文件
#include "mystdio.h"
#include <unistd.h>                         // 用于 sleep() 函数

#define myfile "test.txt"                   // 要写入的测试文件名称

int main()
{
    // 打开文件，以追加模式 ("a") 打开
    _FILE* fp = _fopen(myfile, "a");
    if (fp == NULL)
    {
        return 1;                           // 打开失败则退出
    }

    const char* msg = "hello world\n";      // 写入的字符串内容
    int cnt = 10;                           // 循环次数

    while (cnt)
    {
        _fwrite(fp, msg, strlen(msg));      // 每次写入一行
        // _fflush(fp);                     // 如需每次立即写入可手动刷新
        sleep(1);                           // 每 1 秒写一次
        cnt--;
    }

    _fclose(fp);                            // 写入完毕后关闭文件，释放资源

    return 0;
}
