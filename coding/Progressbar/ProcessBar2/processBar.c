#include "processBar.h"

const char *label = "|-\\";         // 动画符号，用于显示进度条右侧的动态效果
char bar[NUM];                      // 全局进度条数组，用于存储进度条的当前状态

// 初始化进度条，将进度条数组清空
void initbar()
{
    memset(bar, '\0', sizeof(bar)); // 使用memset将进度条数组全部置为'\0'，表示进度条清空
}

// 模拟下载过程，接受回调函数来更新进度条
void download(callback_t cb)
{
    int total = 1000;               // 模拟总任务量（比如：1000MB）
    int curr = 0;                   // 当前已完成的任务量

    while (curr <= total)           // 当当前进度小于总进度时，继续更新
    {
        usleep(50000);              // 模拟一个耗时操作，每次延时50ms
        int rate = curr * 100 / total;  // 根据已完成的任务量计算当前进度百分比
        cb(rate);                   // 调用回调函数更新进度条，传入当前进度百分比
        curr += 10;                 // 增量更新，假设每次下载增加10MB
    }
    printf("\n");                   // 下载完成后输出换行，避免覆盖命令行内容
}

// 更新进度条的函数，根据传入的进度百分比显示进度
void processbar(int rate)
{
    if (rate < 0 || rate > TOP)     // 如果传入的进度值无效（小于0或大于100），直接返回
    {
        return;
    }

    int len = strlen(label);  		// 获取动画符号的长度，用于循环显示旋转符号
    
    // 打印当前进度条状态：[进度条状态][百分比][动画符号]
    // 使用绿色显示进度条部分，恢复默认颜色后显示进度百分比和动画符号
    printf(GREEN "[%-100s]" NONE "[%d%%][%c]\r", bar, rate, label[rate % len]);
    fflush(stdout);  			    // 强制刷新输出缓冲区，确保进度条实时更新

    bar[rate++] = BODY;   			// 在进度条数组的当前进度位置更新为主体符号（'-'）

    if (rate < TOP)
    {
        bar[rate] = RIGHT;			// 如果进度小于最大值，则在尾部添加'>'符号，表示当前进度位置
    }
}

