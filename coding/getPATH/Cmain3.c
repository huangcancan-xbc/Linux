#include <stdio.h>
int main(int argc, char *argv[], char *env[])
{
    printf("获取命令行参数:\n");
    for (int i = 0; i < argc; i++)
    {
        printf("参数[%d]: %s\n", i, argv[i]);
    }

    printf("\n获取环境变量:\n");
    for (int i = 0; env[i] != NULL; i++)
    {
        printf("环境变量[%d]: %s\n", i, env[i]);
    }

    return 0;
}
