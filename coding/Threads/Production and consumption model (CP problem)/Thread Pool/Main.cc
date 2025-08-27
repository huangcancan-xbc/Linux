#include "ThreadPool.hpp"
#include "Task.hpp"
#include <signal.h>

// pthread_spinlock_t slock;                                    // 全局自旋锁
volatile bool running = true;                                   // 信号处理标志，用于优雅退出

// 信号处理函数：捕获Ctrl+C等退出信号
void signalHandler(int signum)
{
    cout << "\n接收到信号 " << signum << "，正在退出..." << endl;
    running = false;
}

int main()
{
    // 注册信号处理函数，捕获Ctrl+C (SIGINT) 和终止信号 (SIGTERM)
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    cout << "线程池启动中..." << endl;
    sleep(1);
    
    ThreadPool<Task>* tp = ThreadPool<Task>::GetInstance();      // 获取线程池单例实例并启动线程池
    tp->Start();  // 启动工作线程
    
    srand(time(nullptr) ^ getpid());                             // 设置随机种子
    
    while(running)                                               // 持续生成任务
    {
        // 1. 构建随机任务
        int x = rand() % 20 + 1;                                 // 生成[1,20]的随机数
        usleep(10);                                              // 微秒延迟，增加随机性
        int y = rand() % 10;                                     // 生成[0,9]的随机数
        char op = opers[rand() % opers.size()];                  // 随机选择操作符

        Task t(x, y, op);                                        // 创建任务对象
        
        // 2. 将任务提交给线程池处理
        tp->Push(t);                                             // 添加任务到线程池
        
        cout << "[主线程] 创建任务: " << t.get_task() << endl;

        sleep(1);                                                // 每1秒生成一个任务
    }
    
    cout << "程序正常退出" << endl;
    return 0;
}