#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
int main()
{
	pid_t id = fork();
	if (id == 0)				// 子进程
	{
		int count = 10;
		while (count--)
		{
			printf("我是子进程...PID:%d, PPID:%d\n", getpid(), getppid());
			sleep(1);
		}
		exit(0);
	}

	int status = 0;
	pid_t ret = wait(&status);
	if (ret > 0)				// 父进程
	{
		// 父进程等待子进程结束
		printf("等待子进程结束...\n");
		if (WIFEXITED(status))
		{
			// 子进程正常结束
			printf("子进程正常结束，退出状态码:%d\n", WEXITSTATUS(status));
		}
	}

	sleep(3);
	return 0;
}
