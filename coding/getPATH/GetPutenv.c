#include <stdio.h>
#include <stdlib.h>
extern int putenv(char *string);		// 添加putenv的显式声明

int main()
{
    // 获取环境变量
    char *home = getenv("HOME");
    if (home)
    {
        printf("HOME 环境变量的值: %s\n", home);
    } 
    else
    {
        printf("HOME 环境变量未设置。\n");
    }

    // 设置环境变量
    putenv("MYVAR=MyCustomValue");
    char *myvar = getenv("MYVAR");
    if (myvar)
    {
        printf("MYVAR 环境变量的值: %s\n", myvar);
    }

    // 修改环境变量
    putenv("MYVAR=NewValue");
    myvar = getenv("MYVAR");
    if (myvar)
    {
        printf("修改后的 MYVAR 环境变量的值: %s\n", myvar);
    }

    return 0;
}
