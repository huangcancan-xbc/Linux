#include <iostream>
#include <pthread.h>
using namespace std;

void* myThread(void* arg)
{
    cout << "子线程正在运行..." << endl;
    pthread_exit((void*)1234);  // 主动退出，返回值是1234

    cout << "子线程退出已经退出了，并且这一句话不会被执行！" << endl;
}

int main()
{
    pthread_t tid;
    void* retval;

    pthread_create(&tid, nullptr, myThread, nullptr);
    pthread_join(tid, &retval);  // 获取子线程返回值

    cout << "子线程退出码：" << (long long)retval << endl;
    // 注意：在64位的Linux中，void*（指针）是 8 字节，所以转成 long long，如果转成 int 会报错！
    return 0;
}