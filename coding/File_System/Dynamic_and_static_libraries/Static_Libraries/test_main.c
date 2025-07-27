#include <stdio.h>
#include "My_static_library.h"  // 包含头文件

int main()
{
    printf("add(3, 4) = %d\n", add(3, 4));
    printf("div(10, 0) = %d\n", div(10, 0));

    if (myerrno == 1)
    {
        printf("Error: Divide by zero\n");
    }

    return 0;
} 
