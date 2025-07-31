#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
    for (int i = 0; i < 3; i++)
    {
        int p[2];
        pipe(p);
        cout << p[0] << ", " << p[1] << endl;
        close(p[0]);                // 关闭后描述符会重新可用
    }

    return 0;
}