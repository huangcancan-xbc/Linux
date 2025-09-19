#include <iostream>
#include <unistd.h>                                  // pipe, fork, read, write
#include <vector>
#include <sys/types.h>                               // pid_t
#include <sys/wait.h>                                // waitpid
#include <fcntl.h>                                   // fcntl
using namespace std;

class ProcessPool
{
    int pool_size;                                   // 进程池大小
    int next = 0;                                    // 任务轮询下标
    vector<pid_t> pids;                              // 存储子进程pid

    vector<vector<int>> task_pipes;                  // 任务管道（父写，子读）
    vector<vector<int>> result_pipes;                // 结果管道（子写，父读）

public:
    explicit ProcessPool(int pool_size);             // 构造函数
    ~ProcessPool();                                  // 析构函数

    void run();                                      // 启动进程池
    void submit_task(int task);                      // 提交任务
    int get_ret();                                   // 获取结果
    void shutdown();                                 // 停止所有子进程
};

// 构造函数
ProcessPool::ProcessPool(int size)
    : pool_size(size)
{
    pids.resize(pool_size);                          // 初始化pid数组
    task_pipes.resize(pool_size);                    // 初始化任务管道
    result_pipes.resize(pool_size);                  // 初始化结果管道 
}

// 启动进程池
void ProcessPool::run()
{
    for (int i = 0; i < pool_size; i++)
    {
        task_pipes[i].resize(2);                     // 创建任务管道
        result_pipes[i].resize(2);                   // 创建结果管道

        if (pipe(task_pipes[i].data()) == -1 || pipe(result_pipes[i].data()) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();                          // 创建子进程
        if (pid == 0)                                // 子进程
        {
            close(task_pipes[i][1]);                 // 关闭父写端
            close(result_pipes[i][0]);               // 关闭子读端

            while (true)                             // 循环等待任务
            {
                int task;                            // 接收任务
                ssize_t n = read(task_pipes[i][0], &task, sizeof(task));        // 读取任务

                if (n <= 0)                          // 达到文件末尾（EOF）或者出错
                {
                    break;
                }

                // 任务处理：
                int ret = task * 5;

                write(result_pipes[i][1], &ret, sizeof(ret));                   // 写入结果
            }

            close(task_pipes[i][0]);                 // 关闭任务管道读端
            close(result_pipes[i][1]);               // 关闭结果管道写端
            _exit(0);                                // 退出子进程
        }
        else if (pid > 0)
        {
            pids[i] = pid;                           // 记录子进程pid
            close(task_pipes[i][0]);                 // 关闭任务管道读端
            close(result_pipes[i][1]);               // 关闭结果管道写端

            int flags = fcntl(result_pipes[i][0], F_GETFL, 0);
            fcntl(result_pipes[i][0], F_SETFL, flags | O_NONBLOCK);
        }
        else
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
}

// 提交任务
void ProcessPool::submit_task(int task)
{
    int cur = next % pool_size;
    cout << "父进程向子进程 " << cur << " 提交任务：" << task << endl;
    write(task_pipes[cur][1], &task, sizeof(task));   // 写入任务
    next++;
}

// 获取结果
int ProcessPool::get_ret()
{
    int ret;
    while (true)
    {
        for (int i = 0; i < pool_size; ++i)
        {
            ssize_t n = read(result_pipes[i][0], &ret, sizeof(ret));

            if (n > 0)
            {
                cout << "父进程从子进程 " << i << " 收到结果：" << ret << endl;
                return ret;
            }
            else if (n < 0 && errno == EAGAIN)
            {
                // 非阻塞下无数据，跳过
                continue;
            }
            // n==0 (EOF) 也跳过
        }

        usleep(1000);                                // 等待1ms避免死循环占满CPU
    }
}

// 停止所有子进程
void ProcessPool::shutdown()
{
    for (int i = 0; i < pool_size; i++)
    {
        close(task_pipes[i][1]);                     // 关闭任务管道写端，子进程读到 EOF 会退出
    }

    for (int i = 0; i < pool_size; i++)
    {
        waitpid(pids[i], NULL, 0);                   // 等待子进程退出
        close(result_pipes[i][0]);                   // 关闭结果管道读端
    }
}

// 析构函数
ProcessPool::~ProcessPool()
{
    shutdown();
}

int main()
{
    ProcessPool pool(3);                             // 启动3个子进程
    pool.run();

    for (int i = 1; i <= 6; ++i)
    {
        pool.submit_task(i);                         // 提交任务1~6
    }

    for (int i = 0; i < 6; ++i)
    {
        int result = pool.get_ret();
        cout << "最终接收到结果：" << result << endl;
    }

    return 0;
}