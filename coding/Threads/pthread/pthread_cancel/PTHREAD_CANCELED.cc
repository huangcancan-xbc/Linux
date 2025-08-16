#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
   while (1)
   {
       cout << "子线程运行中..." << endl;
       sleep(1);
   }
}

int main()
{
   pthread_t tid;
   pthread_create(&tid, nullptr, func, nullptr);

   sleep(2);                            // 让子线程跑一会儿

   pthread_cancel(tid);                 // 取消子线程

   void* retval;
   pthread_join(tid, &retval);          // 等待子线程结束

   // 检查返回值是否为被取消
   if (retval == PTHREAD_CANCELED)
   {
       cout << "子线程被成功取消，返回值 = PTHREAD_CANCELED" << endl;
   }
   else
   {
       cout << "子线程正常退出，返回值 = " << retval << endl;
   }

   return 0;
}