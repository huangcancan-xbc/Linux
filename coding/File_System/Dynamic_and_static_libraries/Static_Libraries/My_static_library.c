#include "My_static_library.h"              // 引入头文件

int myerrno = 0;                            // 定义全局错误码变量

int add(int x, int y)
{
    return x + y;
}

int sub(int x, int y)
{
    return x - y;
}

int mul(int x, int y)
{
    return x * y;
}

int div(int x, int y)
{
    if (y == 0)                             // 检查除数是否为0
    {
        myerrno = 1;                        // 设置错误码
        return -1;                          // 返回错误标识
    }

    return x / y;
}
