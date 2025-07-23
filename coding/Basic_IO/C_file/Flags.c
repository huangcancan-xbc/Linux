#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    umask(0);
    int fd = open("temp.txt", O_WRONLY | O_CREAT, 0666);
    if (fd < 0)
    {
        printf("open file error\n");
        return 1;
    }

    return 0;
}
