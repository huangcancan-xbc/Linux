#include <stdio.h>
#include <stdlib.h>

int global_init = 10;               // 数据段
int global_uninit;                  // BSS段

int main()
{
    int stack_var;                  // 栈
    static int static_init = 20;    // 数据段
    static int static_uninit;       // BSS段
    char* heap_var = malloc(10);    // 堆

    printf("文本段（代码）: %p\n", (void*)main);
    printf("数据段（已初始化全局变量）: %p\n", (void*)&global_init);
    printf("数据段（已初始化静态变量）: %p\n", (void*)&static_init);
    printf("BSS段（未初始化全局变量）: %p\n", (void*)&global_uninit);
    printf("BSS段（未初始化静态变量）: %p\n", (void*)&static_uninit);
    printf("堆（动态分配内存）: %p\n", (void*)heap_var);
    printf("栈（局部变量）: %p\n", (void*)&stack_var);

    free(heap_var);
    return 0;
}
