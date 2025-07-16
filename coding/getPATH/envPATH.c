#include <stdio.h>
extern char **environ;
int main()
{
    printf("通过 environ 获取环境变量:\n");
    for (int i = 0; environ[i] != NULL; i++)
    {
        printf("环境变量[%d]: %s\n", i, environ[i]);
    }

    return 0;
}
