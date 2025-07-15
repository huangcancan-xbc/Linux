#include "processBar.h"

const char *label = "|-\\";           	   // 旋转动画符号, 用于表示进度条右侧的动态符号

// 更新进度条函数
void processbar(int speed)
{
    char bar[NUM];                         // 创建一个字符数组，用来存储进度条的状态
    memset(bar, '\0', sizeof(bar));        // 初始化进度条数组，填充空字符
    int len = strlen(label);               // 获取动画符号的长度，label长度用于循环动画符号

    int cnt = 0;                           // 进度条的当前进度，初始为0
    while (cnt <= TOP)                     // 当进度小于等于最大值（100%）时，持续循环
    {  
        // 在进度条的前面输出进度状态，格式：[进度条状态][百分比][动画符号]
        printf("[%-100s][%d%%][%c]\r", bar, cnt, label[cnt % len]);
        fflush(stdout);                    // 强制刷新输出缓冲区，以确保进度条实时更新显示
        bar[cnt++] = BODY;                 // 将进度条数组的当前进度位置更新为进度条主体符号

        if (cnt < TOP)                     // 如果当前进度小于最大值，则更新尾部符号
        {
            bar[cnt] = RIGHT;              // 在进度条的尾部添加'>'符号，表示当前进度
        }

        usleep(speed);                     // 使用usleep函数控制更新速度，单位为微秒
    }
    
    printf("\n");                          // 完成进度条后换行，避免覆盖命令行内容
}
