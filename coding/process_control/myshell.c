#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

// ======================== 宏定义区域 ========================
#define LEFT "["                            // shell 显示用的左括号
#define RIGHT "]"                           // shell 显示用的右括号
#define LABLE "#"                           // shell 显示用的提示符号
#define DELIM " \t"                         // 分隔符（空格和制表符）
#define LINE_SIZE 1024                      // 每一行命令最大长度
#define ARGC_SIZE 32                        // 最大命令参数个数
#define EXIT_CODE 44                        // 子进程执行失败退出码

// 重定向类型定义
#define NONE -1
#define IN_RDIR     0                       // 输入重定向（<）
#define OUT_RDIR    1                       // 输出重定向（>）
#define APPEND_RDIR 2                       // 追加输出重定向（>>）

// ======================== 全局变量 ========================
int lastcode = 0;                           // 上一次命令的返回码
int quit = 0;                               // 控制是否退出 shell
extern char** environ;                      // 系统环境变量表
char commandline[LINE_SIZE];                // 存储用户输入命令
char* argv[ARGC_SIZE];                      // 存储分割后的命令参数
char pwd[LINE_SIZE];                        // 当前工作目录
char* rdirfilename = NULL;                  // 重定向的文件名
int rdir = NONE;                            // 当前重定向类型

char myenv[LINE_SIZE];                      // 存储 export 设置的环境变量

// ======================== 工具函数 ========================

// 获取当前用户名
const char* getusername()
{
    return getenv("USER");
}

// 获取主机名
const char* gethostname1()
{
    return getenv("HOSTNAME");
}

// 获取当前路径（PWD）
void getpwd()
{
    getcwd(pwd, sizeof(pwd));
}

// 解析重定向符号（< > >>）并设置 rdir 和 rdirfilename
void check_redir(char* cmd)
{
    char* pos = cmd;
    while (*pos)
    {
        if (*pos == '>')
        {
            if (*(pos + 1) == '>')          // >> 追加重定向
            { 
                *pos++ = '\0';
                *pos++ = '\0';
                while (isspace(*pos))
                {
                    pos++;
                }

                rdirfilename = pos;
                rdir = APPEND_RDIR;
                break;
            }
            else                            // > 普通输出重定向
            { 
                *pos = '\0';
                pos++;
                while (isspace(*pos))
                {
                    pos++;
                }

                rdirfilename = pos;
                rdir = OUT_RDIR;
                break;
            }
        }
        else if (*pos == '<')               // < 输入重定向
        {
            *pos = '\0';
            pos++;
            while (isspace(*pos))
            {
                pos++;
            }

            rdirfilename = pos;
            rdir = IN_RDIR;
            break;
        }
        pos++;
    }
}

// 显示 shell 提示符并获取用户输入命令
void interact(char* cline, int size)
{
    getpwd();
    printf(LEFT"%s@%s %s"RIGHT""LABLE" ", getusername(), gethostname1(), pwd);
    char* s = fgets(cline, size, stdin);    // 获取用户输入
    assert(s);                              // 确保输入成功
    (void)s;                                // 显式标记该变量已被“使用”，从而抑制编译器警告（这是一种代码规范技巧，表明有意忽略此变量）

    cline[strlen(cline) - 1] = '\0';        // 去除换行符
    check_redir(cline);                     // 检查是否有重定向
}

// 将命令行字符串根据空格分割成参数数组
int splitstring(char cline[], char* _argv[])
{
    int i = 0;
    argv[i++] = strtok(cline, DELIM);
    while (_argv[i++] = strtok(NULL, DELIM));   // 使用 strtok 循环分割

    return i - 1;
}

// 执行普通命令（fork+exec）
void NormalExcute(char* _argv[])
{
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork");
        return;
    }
    else if (id == 0)                           // 子进程执行命令
    {
        int fd = 0;
        if (rdir == IN_RDIR)
        {
            fd = open(rdirfilename, O_RDONLY);
            dup2(fd, 0);                        // 标准输入重定向
        }
        else if (rdir == OUT_RDIR)
        {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            dup2(fd, 1);                        // 标准输出重定向
        }
        else if (rdir == APPEND_RDIR) {
            fd = open(rdirfilename, O_CREAT | O_WRONLY | O_APPEND, 0666);
            dup2(fd, 1);                        // 标准输出追加重定向
        }

        execvp(_argv[0], _argv);                // 执行命令（从 PATH 路径中查找）
        exit(EXIT_CODE);                        // exec 出错则退出
    }
    else                                        // 父进程等待子进程结束
    {
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        if (rid == id)
        {
            lastcode = WEXITSTATUS(status);     // 记录子进程退出码
        }
    }
}

// 构建内置命令：cd, export, echo
int buildCommand(char* _argv[], int _argc)
{
    if (_argc == 2 && strcmp(_argv[0], "cd") == 0)
    {
        chdir(argv[1]);                         // 切换工作目录
        getpwd();
        sprintf(getenv("PWD"), "%s", pwd);      // 更新环境变量 PWD
        return 1;
    }
    else if (_argc == 2 && strcmp(_argv[0], "export") == 0)
    {
        strcpy(myenv, _argv[1]);
        putenv(myenv);                          // 添加或修改环境变量
        return 1;
    }
    else if (_argc == 2 && strcmp(_argv[0], "echo") == 0)
    {
        if (strcmp(_argv[1], "$?") == 0)
        {
            printf("%d\n", lastcode);           // 打印上一次命令的返回码
            lastcode = 0;
        }
        else if (*_argv[1] == '$')
        {
            char* val = getenv(_argv[1] + 1);   // 获取环境变量值
            if (val)
            {
                printf("%s\n", val);
            }
        }
        else
        {
            printf("%s\n", _argv[1]);
        }
        return 1;
    }

    // 针对 ls 增加颜色选项
    if (strcmp(_argv[0], "ls") == 0)
    {
        _argv[_argc++] = "--color"; // 自动加上颜色显示
        _argv[_argc] = NULL;
    }
    return 0;
}

// ======================== 主函数入口 ========================
int main()
{
    while (!quit)
    {
        // 初始化重定向信息
        rdirfilename = NULL;
        rdir = NONE;

        // 获取用户输入的命令行
        interact(commandline, sizeof(commandline));

        // 分割命令行为参数数组
        int argc = splitstring(commandline, argv);
        if (argc == 0) continue;

        // 处理内置命令
        int n = buildCommand(argv, argc);

        // 执行普通命令
        if (!n)
        {
            NormalExcute(argv);
        }
    }

    return 0;
}

