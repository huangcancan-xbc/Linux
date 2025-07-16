#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *env = getenv("MYENV");  // 获取环境变量
    if (env)
    {
        printf("%s\n", env);  // 如果环境变量存在，则打印
    }
    return 0;
}
