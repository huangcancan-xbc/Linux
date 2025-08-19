#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
using namespace std;

// 枚举线程运行状态
enum Status
{
    OK = 0,             // 正常
    ERROR               // 异常
};

class ThreadTask
{
public:
    // 构造函数：传入线程名字、起始数、结束数、线程编号
    ThreadTask(const string& name, int begin, int end, int id)
        : _name(name), _begin(begin), _end(end), _id(id),
          _result(0), _status(Status::OK)
    {}

    ~ThreadTask() {}    // 析构函数

    // 线程的实际任务函数
    void run()
    {
        for (int i = _begin; i <= _end; ++i)
        {
            _result += i;           // 计算指定区间的和
        }

        printf("线程[%s]运行完毕，计算[%d~%d]结束\n", _name.c_str(), _begin, _end);

        // pthread_exit：线程结束时返回一个指针（被主线程pthread_join获取）
        pthread_exit(this);
    }
public:
    string _name;           // 线程名称
    int _begin;             // 区间起点
    int _end;               // 区间终点
    int _id;                // 线程编号
    int _result;            // 计算结果
    Status _status;         // 线程状态
};

// 线程入口函数（全局函数 / 静态函数）
void* threadEntry(void* arg)
{
    // static_cast<T*> 是 C++ 的类型转换运算符：用于在编译时进行类型安全的指针转换
    ThreadTask* task = static_cast<ThreadTask*>(arg);

    task->run();                                // 执行实际任务（内部调用 pthread_exit 退出线程）
    return nullptr;                             // 实际不会执行到这里，因为 run() 中直接退出了线程
}

int main()
{
    const int THREAD_NUM = 4;                   // 总线程数量
    pthread_t tids[THREAD_NUM];                 // 线程ID数组
    ThreadTask* tasks[THREAD_NUM];              // 每个线程绑定一个任务对象

    int range = 100;                            // 每个线程负责计算100个数的和
    int start = 1;                              // 区间起始点

    for (int i = 0; i < THREAD_NUM; ++i)        // 创建线程
    {
        string name = "Thread-" + to_string(i + 1);
        int end = start + range - 1;

        tasks[i] = new ThreadTask(name, start, end, i + 1);     // 创建一个任务对象

        // 创建线程，执行 threadEntry 函数，参数传入任务对象指针
        pthread_create(&tids[i], nullptr, threadEntry, tasks[i]);

        start = end + 1;
        sleep(1);                                // 保证顺序输出（演示效果）
    }

    for (int i = 0; i < THREAD_NUM; ++i)         // 等待线程结束 + 收集结果
    {
        void* ret = nullptr;

        pthread_join(tids[i], &ret);             // pthread_join 等待线程退出，并获取其返回值

        ThreadTask* task = static_cast<ThreadTask*>(ret);

        if (task->_status == Status::OK)
        {
            printf("%s计算[%d~%d]结果: %d\n", task->_name.c_str(), task->_begin, task->_end, task->_result);
        }
        else
        {
            printf("%s执行失败！\n", task->_name.c_str());
        }

        delete task;                            // 释放任务对象资源
    }

    cout << "所有线程协作完成！" << endl;
    return 0;
}