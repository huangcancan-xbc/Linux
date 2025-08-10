#include <stdio.h>
#include <signal.h>
using namespace std;

volatile int flag = 0;

void handler(int signo)
{
	printf("收到信号:%d\n", signo);
	flag = 1;                           // 设置flag为1，表示收到信号（// 信号处理函数中修改 flag 的值）
}
int main()
{
	signal(SIGINT, handler);            // 注册信号处理函数
	while (!flag);                      // 等待信号处理函数执行完毕
	printf("进程正常退出！\n");

	return 0;
}