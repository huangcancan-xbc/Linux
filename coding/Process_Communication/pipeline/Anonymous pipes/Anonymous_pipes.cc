#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
    int pipefd[2] = { 5,6 };
    int n = pipe(pipefd);

    if (n < 0)
    {
        return 1;
    }

    cout << "pipefd[0]: " << pipefd[0] << " , pipefd[1]: " << pipefd[1] << endl;

    return 0;
}