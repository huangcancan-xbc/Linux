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
int lastcode = 0;                                   // 存储上次命令的退出码
int quit = 0;                                       // 控制主循环的标志
extern char** environ;                              // 外部环境变量指针
char commandline[LINE_SIZE];                        // 存储命令行输入的缓冲区
char* argv[ARGC_SIZE];                              // 存储命令参数的数组
char pwd[LINE_SIZE];                                // 存储当前工作目录的缓冲区
char* rdirfilename = NULL;                          // 存储重定向文件名的指针
int rdir = NONE;                                    // 存储重定向类型的变量

// 自定义环境变量缓冲区
char myenv[LINE_SIZE];                              // 存储自定义环境变量的缓冲区

// 获取用户名的函数
const char* getusername()
{
    return getenv("USER");                          // 从环境变量中获取用户名
}

// 获取主机名的函数
const char* gethostname1()
{
    return getenv("HOSTNAME");                      // 从环境变量中获取主机名
}

// 获取当前工作目录的函数
void getpwd()
{
    getcwd(pwd, sizeof(pwd));                       // 调用系统函数获取当前目录
}

// 检查命令中是否包含重定向的函数
void check_redir(char* cmd)
{
    char* pos = cmd;
    while (*pos)
    {
        if (*pos == '>')                            // 检查输出重定向
        {
            if (*(pos + 1) == '>')
            {
                *pos++ = '\0';                      // 终止命令字符串
                *pos++ = '\0';                      // 跳过 '>>'

                while (isspace(*pos))
                {
                    pos++;                          // 跳过空白字符
                }

                rdirfilename = pos;                 // 设置重定向文件名
                rdir = APPEND_RDIR;                 // 设置追加重定向

                break;
            }
            else
            {
                *pos = '\0';                        // 终止命令字符串
                pos++;

                while (isspace(*pos))
                {
                    pos++;                          // 跳过空白字符
                }

                rdirfilename = pos;                 // 设置重定向文件名
                rdir = OUT_RDIR;                    // 设置输出重定向
                break;
            }
        }
        else if (*pos == '<')                       // 检查输入重定向
        {
            *pos = '\0';                            // 终止命令字符串
            pos++;

            while (isspace(*pos))
            {
                pos++;                              // 跳过空白字符
            }

            rdirfilename = pos;                     // 设置重定向文件名
            rdir = IN_RDIR;                         // 设置输入重定向
            break;
        }

        pos++;
    }
}

// 与用户交互获取命令的函数
void interact(char* cline, int size)
{
    getpwd();                                       // 获取当前工作目录

    // 打印带用户名、主机名和目录的提示符
    printf(LEFT"%s@%s %s"RIGHT""LABLE" ", getusername(), gethostname1(), pwd);
    char* s = fgets(cline, size, stdin);            // 读取用户输入

    assert(s);                                      // 确保输入成功
    (void)s;                                        // 忽略未使用变量警告

    if (strlen(cline) > 0 && cline[strlen(cline) - 1] == '\n')      // 移除输入字符串末尾的换行符
    {
        cline[strlen(cline) - 1] = '\0';
    }

    check_redir(cline);                             // 检查命令中的重定向
}

// 将命令字符串分割为参数数组的函数
int splitstring(char cline[], char* _argv[])
{
    int i = 0;
    _argv[i++] = strtok(cline, DELIM);              // 获取第一个参数
    while ((_argv[i++] = strtok(NULL, DELIM)) != NULL);             // 获取后续参数
    return i - 1;                                   // 返回参数数量
}

// 执行普通命令的函数
void NormalExcute(char* _argv[])
{
    pid_t id = fork();                              // 创建子进程
    if (id < 0)
    {
        perror("fork");                             // 处理 fork 失败
        return;
    }
    else if (id == 0)
    {
        int fd = 0;

        if (rdir == IN_RDIR)                        // 处理输入重定向
        {
            fd = open(rdirfilename, O_RDONLY);      // 打开文件用于读取
            if (fd < 0)
            {
                perror("open");
                exit(EXIT_CODE);
            }
            dup2(fd, 0);                            // 复制文件描述符到标准输入
            close(fd);                              // 关闭原始文件描述符
        }
        else if (rdir == OUT_RDIR)                  // 处理输出重定向
        {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);    // 打开文件用于写入

            if (fd < 0)
            {
                perror("open");
                exit(EXIT_CODE);
            }

            dup2(fd, 1);                            // 复制文件描述符到标准输出
            close(fd);                              // 关闭原始文件描述符
        }
        else if (rdir == APPEND_RDIR)               // 处理追加重定向
        {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_APPEND, 0666);   // 打开文件用于追加

            if (fd < 0)
            {
                perror("open");
                exit(EXIT_CODE);
            }

            dup2(fd, 1);                            // 复制文件描述符到标准输出
            close(fd);                              // 关闭原始文件描述符
        }

        // 执行命令
        execvp(_argv[0], _argv);
        perror("execvp");
        exit(EXIT_CODE);                            // 使用自定义代码退出
    }
    else
    {
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);        // 等待子进程完成

        if (rid == id)
        {
            if (WIFEXITED(status))
            {
                lastcode = WEXITSTATUS(status);     // 存储退出状态
            }
            else
            {
                lastcode = -1;                      // 表示非正常退出
            }
        }
    }
}

// 处理内置命令的函数
int buildCommand(char* _argv[], int _argc)
{
    if (_argc == 2 && strcmp(_argv[0], "cd") == 0)  // 处理 'cd' 命令
    {
        if (chdir(_argv[1]) != 0)                   // 更改工作目录
        {
            perror("chdir");
            return 1;                               // 表示已处理命令
        }

        getpwd();                                   // 更新当前目录
        char newpwd[LINE_SIZE];
        getcwd(newpwd, sizeof(newpwd));
        setenv("PWD", newpwd, 1);                   // 更新 PWD 环境变量

        return 1;                                   // 表示已处理命令
    }
    else if (_argc == 2 && strcmp(_argv[0], "export") == 0)     // 处理 'export' 命令
    {
        putenv(_argv[1]);                           // 添加到环境变量
        return 1;                                   // 表示已处理命令
    }
    else if (_argc == 2 && strcmp(_argv[0], "echo") == 0)       // 处理 'echo' 命令
    {
        if (strcmp(_argv[1], "$?") == 0)
        {
            printf("%d\n", lastcode);               // 打印上次命令退出码
            lastcode = 0;                           // 重置退出码
        }
        else if (*_argv[1] == '$')
        {
            char* val = getenv(_argv[1] + 1);       // 获取环境变量值
            if (val) printf("%s\n", val);           // 打印变量值
        }
        else
        {
            printf("%s\n", _argv[1]);               // 打印参数
        }

        return 1;                                   // 表示已处理命令
    }

    if (strcmp(_argv[0], "ls") == 0)                // 特殊处理 'ls' 命令以启用颜色
    {
        for (int i = _argc; i < ARGC_SIZE - 1; i++)
        {
            _argv[i] = "--color";                   // 添加颜色选项
            _argv[i + 1] = NULL;                    // 终止参数数组
            break;
        }
    }

    return 0;                                       // 表示未处理命令（将执行普通命令）
}

// 主函数
int main()
{
    while (!quit)
    {
        rdirfilename = NULL;                        // 重置重定向文件名
        rdir = NONE;                                // 重置重定向类型
        interact(commandline, sizeof(commandline)); // 获取用户命令
        int argc = splitstring(commandline, argv);  // 分割命令为参数

        if (argc == 0)
        {
            continue;                               // 跳过空命令
        }
        int n = buildCommand(argv, argc);           // 检查是否为内置命令

        if (!n)
        {
            NormalExcute(argv);                     // 执行普通命令
        }
    }
    return 0;
}

