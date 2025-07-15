#include "processBar.h"

int main()
{
    // 模拟下载1，调用download函数进行下载并更新进度条
    printf("Download 1:\n");
    download(processbar);    		// 调用download函数，并将processbar作为回调函数传入
    initbar();                		// 下载完成后重置进度条

    // 模拟下载2，重复下载过程并重置进度条
    printf("Download 2:\n");
    download(processbar);    		// 第二次下载
    initbar();                		// 重置进度条

    // 模拟下载3，最后一次下载
    printf("Download 3:\n");
    download(processbar);    		// 第三次下载

    return 0;                 		// 程序结束
}
