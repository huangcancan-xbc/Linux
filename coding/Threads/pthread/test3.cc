#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <string>
using namespace std;

#define NUM 3

// __thread 是 GCC 提供的线程局部存储关键字，用于声明在函数内的变量，每个线程都有自己的副本，互不干扰。
__thread unsigned int thread_local_number = 0;     // 每个线程独有
__thread int thread_local_pid = 0;                 // 每个线程独有

struct threadData                                  // 线程数据结构
{
    string threadname;                             // 线程名称
    int thread_id;                                 // 线程编号
};

string toHex(pthread_t tid)                        // 将pthread_t转换为十六进制字符串（便于显示）
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "0x%lx", tid);// 使用%lx格式化pthread_t
    return buffer;
}

void InitThreadData(threadData* td, int number)    // 初始化线程数据
{
    td->threadname = "thread-" + to_string(number);// 设置线程名称
    td->thread_id = number;                        // 设置线程ID
}

void* threadRoutine(void* args)                    // 线程执行函数 - 所有线程都执行这个函数，但每个线程的数据是独立的
{
    pthread_detach(pthread_self());                // 立即将当前线程设置为分离状态，这样线程结束后会自动释放资源，无需主线程join

    threadData* td = static_cast<threadData*>(args);

    // 获取当前线程ID和进程ID
    string tid_str = toHex(pthread_self());        // 当前线程的ID
    int process_pid = getpid();                    // 进程ID（所有线程共享）

    // 初始化线程局部变量
    thread_local_number = td->thread_id * 100;     // 每个线程有不同的值
    thread_local_pid = process_pid;                // 每个线程都有自己的副本

    cout << "[" << td->threadname << "] 启动！"
        << "线程ID: " << tid_str
        << ", 进程ID: " << process_pid
        << ", 线程局部变量 number: " << thread_local_number << endl;

    for (int i = 0; i < 5; i++)                    // 减少循环次数便于观察
    {
        // 展示线程的关键特性：
        // 1. 同一进程内的所有线程共享进程ID
        // 2. 每个线程有自己的线程ID
        // 3. 每个线程有自己的线程局部存储变量
        cout << "[" << td->threadname << "] " << "第" << (i + 1) << "次执行："
            << "线程ID: " << tid_str
            << ", 进程ID: " << process_pid
            << ", 局部number: " << thread_local_number
            << ", 局部pid: " << thread_local_pid << endl;

        sleep(1);                                   // 休眠1秒
        thread_local_number++;                      // 修改线程局部变量（只影响当前线程）
    }

    cout << "[" << td->threadname << "] 执行完毕!" << endl;

    delete td;                                      // 清理分配的内存
    return nullptr;                                 // 返回nullptr表示线程正常结束
}

int main()
{
    cout << "主线程ID: " << toHex(pthread_self()) << endl;
    cout << "进程ID: " << getpid() << endl << endl;

    vector<pthread_t> tids;                         // 存储所有线程ID的容器

    cout << "开始创建 " << NUM << " 个线程..." << endl;

    for (int i = 0; i < NUM; i++)                   // 创建多个线程
    {
        pthread_t tid;                              // 线程ID变量
        threadData* td = new threadData;            // 为每个线程分配独立的数据
        InitThreadData(td, i);                      // 初始化线程数据

        int ret = pthread_create(&tid, nullptr, threadRoutine, td);
        if (ret != 0)
        {
            cerr << "创建线程失败: " << strerror(ret) << endl;
            delete td;                              // 创建失败时释放内存
            continue;
        }

        cout << "成功创建线程 " << td->threadname << " (ID: " << toHex(tid) << ")" << endl;

        tids.push_back(tid);                        // 保存线程ID
        usleep(100000);                             // 延时0.1秒，确保线程正确启动
    }

    cout << "\n所有线程创建完成，主线程等待..." << endl;
    sleep(2);                                       // 给线程一些执行时间
    
    cout << "\n=== 尝试对分离线程进行join操作 ===" << endl;     // 尝试对分离线程进行join操作（这会失败）
    for (size_t i = 0; i < tids.size(); i++)
    {
        int result = pthread_join(tids[i], nullptr);
        printf("对线程 0x%lx 执行join的结果: %d (%s)\n", tids[i], result, strerror(result));
        // 由于线程已被分离，这里会返回EINVAL（无效参数）
    }

    cout << "1. 线程共享进程资源（如进程ID、文件描述符等）" << endl;
    cout << "2. 每个线程有自己的线程ID和栈空间" << endl;
    cout << "3. __thread关键字创建线程局部存储变量" << endl;
    cout << "4. 分离线程结束后自动释放资源" << endl;
    cout << "5. 分离线程不能被pthread_join回收" << endl;
    cout << "6. 线程执行是并发的，输出顺序不同" << endl;

    cout << "\n主线程继续执行中..." << endl;
    sleep(5);

    cout << "主线程执行完毕，程序退出！" << endl;

    return 0;
}