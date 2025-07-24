#include <stdio.h>                                  // 包含输入输出函数 printf, fgets
#include <stdlib.h>                                 // 包含标准库函数 malloc, exit
#include <string.h>                                 // 包含字符串操作函数 strcpy, strlen
#include <assert.h>                                 // 包含断言函数 assert
#include <unistd.h>                                 // 包含 POSIX 操作函数 fork, getcwd
#include <sys/types.h>                              // 包含系统数据类型定义
#include <sys/wait.h>                               // 包含进程等待函数 waitpid
#include <ctype.h>                                  // 包含字符处理函数 isdigit
#include <fcntl.h>                                  // 包含文件控制函数 open

#define LEFT "["                                    // 定义提示符左括号
#define RIGHT "]"                                   // 定义提示符右括号
#define LABLE "#"                                   // 定义提示符标签
#define DELIM " \t"                                 // 定义字符串分割符
#define LINE_SIZE 1024                              // 定义命令行最大长度
#define ARGC_SIZE 32                                // 定义参数最大数量
#define EXIT_CODE 44                                // 定义子进程退出代码

#define NONE -1                                     // 定义无重定向
#define IN_RDIR 0                                   // 定义输入重定向
#define OUT_RDIR 1                                  // 定义输出重定向
#define APPEND_RDIR 2                               // 定义追加重定向

// 全局变量声明
int lastcode = 0;                                   // 上次命令的退出码
int quit = 0;                                       // 主循环控制标志
extern char** environ;                              // 外部环境变量
char commandline[LINE_SIZE];                        // 命令行输入缓冲区
char* argv[ARGC_SIZE];                              // 参数数组
char pwd[LINE_SIZE];                                // 当前工作目录缓冲区
char* rdirfilename = NULL;                          // 重定向文件名
int rdir = NONE;                                    // 重定向类型
char myenv[LINE_SIZE];                              // 自定义环境变量缓冲区

// 获取用户名
const char* getusername()
{
    return getenv("USER");                          // 从环境变量获取用户名
}

// 获取主机名
const char* gethostname1()
{
    static char hostname[LINE_SIZE];
    gethostname(hostname, sizeof(hostname));        // 使用系统调用获取主机名
    return hostname;
}

// 获取当前工作目录
void getpwd()
{
    getcwd(pwd, sizeof(pwd));                       // 系统调用获取工作目录
}

// 检查命令中是否存在重定向符号
void check_redir(char* cmd)
{
    char* pos = cmd;
    while (*pos)
    {
        if (*pos == '>')                            // 检查输出重定向 >
        {
            *pos++ = '\0';
            if (*pos == '>')
            {
                *pos++ = '\0';                      // 检查追加 >>
                rdir = APPEND_RDIR;
            }
            else
            {
                rdir = OUT_RDIR;
            }
            while (isspace(*pos)) pos++;             // 跳过空格
            rdirfilename = pos;                      // 保存重定向文件名
            break;
        }
        else if (*pos == '<')                        // 检查输入重定向 <
        {
            *pos++ = '\0';
            rdir = IN_RDIR;
            while (isspace(*pos)) pos++;
            rdirfilename = pos;
            break;
        }
        pos++;
    }
}

// 与用户交互获取命令
void interact(char* cline, int size)
{
    getpwd();                                        // 获取当前目录
    printf(LEFT"%s@%s %s"RIGHT""LABLE" ", getusername(), gethostname1(), pwd);
    char* s = fgets(cline, size, stdin);             // 读取用户输入
    assert(s);                                       // 确保输入成功
    (void)s;                                         // 忽略未使用变量警告
    cline[strcspn(cline, "\n")] = '\0';              // 移除末尾换行符
    check_redir(cline);                              // 检查重定向
}

// 分割命令字符串为参数
int splitstring(char cline[], char* _argv[])
{
    int i = 0;
    char* token = strtok(cline, DELIM);
    while (token && i < ARGC_SIZE - 1)
    {
        _argv[i++] = token;
        token = strtok(NULL, DELIM);
    }
    _argv[i] = NULL;                                 // 参数数组以 NULL 结尾
    return i;
}

// 执行普通命令
void NormalExecute(char* _argv[])
{
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork");
        return;
    }
    else if (id == 0)                                // 子进程
    {
        int fd = -1;
        if (rdir == IN_RDIR)
        {
            fd = open(rdirfilename, O_RDONLY);
            if (fd >= 0) { dup2(fd, 0); close(fd); }
            else { perror("open"); exit(EXIT_CODE); }
        }
        else if (rdir == OUT_RDIR)
        {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (fd >= 0) { dup2(fd, 1); close(fd); }
            else { perror("open"); exit(EXIT_CODE); }
        }
        else if (rdir == APPEND_RDIR)
        {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_APPEND, 0666);
            if (fd >= 0) { dup2(fd, 1); close(fd); }
            else { perror("open"); exit(EXIT_CODE); }
        }

        execvp(_argv[0], _argv);
        perror("execvp");                             // execvp失败则打印错误
        exit(EXIT_CODE);
    }
    else                                              // 父进程
    {
        int status = 0;
        waitpid(id, &status, 0);                      // 等待子进程结束
        lastcode = WEXITSTATUS(status);               // 记录退出码
    }
}

// 处理内置命令
int buildCommand(char* _argv[], int _argc)
{
    if (_argc == 2 && strcmp(_argv[0], "cd") == 0)
    {
        if (chdir(_argv[1]) == 0)                     // 切换目录
        {
            getpwd();
            setenv("PWD", pwd, 1);                    // 更新 PWD 环境变量
        }
        else
        {
            perror("chdir");
        }
        return 1;
    }
    else if (_argc == 2 && strcmp(_argv[0], "export") == 0)
    {
        strcpy(myenv, _argv[1]);
        putenv(myenv);                                // 添加到环境变量
        return 1;
    }
    else if (_argc == 2 && strcmp(_argv[0], "echo") == 0)
    {
        if (strcmp(_argv[1], "$?") == 0)
        {
            printf("%d\n", lastcode);
        }
        else if (_argv[1][0] == '$')
        {
            char* val = getenv(_argv[1] + 1);
            if (val) printf("%s\n", val);
        }
        else
        {
            printf("%s\n", _argv[1]);
        }
        return 1;
    }
    if (strcmp(_argv[0], "ls") == 0)
    {
        _argv[_argc++] = "--color";                   // ls 增加彩色显示
        _argv[_argc] = NULL;
    }
    return 0;                                          // 非内置命令
}

// 主函数
int main()
{
    while (!quit)
    {
        rdirfilename = NULL;
        rdir = NONE;
        interact(commandline, sizeof(commandline));    // 获取用户输入
        int argc = splitstring(commandline, argv);     // 分割参数
        if (argc == 0) continue;                       // 跳过空命令

        int isbuild = buildCommand(argv, argc);
        if (!isbuild)
        {
            NormalExecute(argv);                       // 执行普通命令
        }
    }
    return 0;
}
