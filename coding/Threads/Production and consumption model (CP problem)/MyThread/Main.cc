#include "MyThread.hpp"
#include <vector>
#include <unistd.h>

int threads_num = 3;

void print()
{
    int count = 0;
    while(count < threads_num)
    {
        cout << "我是一个 C++ 语言层面上封装的线程！" << "代号是：" << count++ << endl;
        sleep(1);
    }

    cout << "所有线程都结束了！" << endl;
}

int main()
{
    vector<Thread> threads;
    for (int i = 0; i < threads_num; ++i)
    {
        threads.push_back(Thread(print));
    }

    cout << "开始启动所有线程喽~" << endl;

    for(auto &x : threads)
    {
        x.run();
        cout << "线程 " << x.name() << " 启动成功！其时间戳的值是：" << x.start_timestamp() << endl;
    }

    for(auto &x : threads)
    {
        x.join();
    }

    cout << "所有线程都结束了！" << endl;

    return 0;
}
