#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

int main()
{
    while(true)
    {
        cout << "守护进程 temp 正在运行..." << endl;
        sleep(1);
    }
    return 0;
}