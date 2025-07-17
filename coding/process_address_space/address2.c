#include <stdio.h>
#include <stdlib.h>
int g_unval;                            // 未初始化全局变量（BSS段）
int g_val = 100;                        // 已初始化全局变量（数据段）
const int g_const = 200;                // 只读常量（可能位于文本段或只读数据段）

int main(int argc, char* argv[], char* envp[])
{
    // 1. 代码段和只读数据
    printf("==================== 代码段和只读数据 ====================\n");
    printf("main函数地址（代码段）          : %p\n", main);
    printf("字符串常量地址（只读数据）       : %p\n", "hello world");

    // 2. 数据段和BSS段
    printf("\n==================== 数据段和BSS段 ====================\n");
    printf("已初始化全局变量（g_val）        : %p\n", &g_val);
    printf("未初始化全局变量（g_unval）      : %p\n", &g_unval);
    printf("const全局常量（g_const）        : %p\n", &g_const);

    // 3. 堆区
    printf("\n==================== 堆区 ====================\n");
    int* p1 = (int*)malloc(10);
    int* p2 = (int*)malloc(20);
    printf("小堆块地址（p1）                : %p\n", p1);
    printf("大堆块地址（p2）                : %p\n", p2);
    printf("堆生长方向（p2 - p1）           : %ld\n", p2 - p1);  // 正数表示向高地址生长

    // 4. 栈区
    printf("\n==================== 栈区 ====================\n");
    int stack_var1;
    int stack_var2;
    printf("栈变量1（stack_var1）           : %p\n", &stack_var1);
    printf("栈变量2（stack_var2）           : %p\n", &stack_var2);
    printf("栈生长方向（&stack_var2 - &stack_var1）: %ld\n",
        (long)&stack_var2 - (long)&stack_var1);  // 负数表示向低地址生长

    // 5. 命令行参数和环境变量
    printf("\n==================== 命令行参数和环境变量 ====================\n");
    printf("argv数组地址                    : %p\n", argv);
    printf("envp数组地址                    : %p\n", envp);

    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]地址                   : %p\n", i, argv[i]);
    }

    for (int i = 0; envp[i]; i++)
    {
        printf("envp[%d]地址                   : %p\n", i, envp[i]);
    }

    // 6. 动态库加载地址
    printf("\n==================== 动态库 ====================\n");
    void* libc_addr = (void*)printf;
    printf("libc函数（printf）地址          : %p\n", libc_addr);

    free(p1);
    free(p2);
    return 0;
}
