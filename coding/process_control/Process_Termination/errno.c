#include<stdio.h> 
#include<unistd.h>                                                                                                                                                                                           
#include<string.h>
#include<stdlib.h>
#include<errno.h>                                       //注意要带好头文件
int main()
{
    int ret = 0;
    char* p = (char*)malloc(1000 * 1000 * 1000 * 4);    //这个扩容肯定会出错的，因为扩容空间太大了
    if (p == NULL)
    {
        printf("mallo error, %d:%s\n", errno, strerror(errno));   //errno会记录错误码，将它传到strerror中就可以得到错误信息
        ret = errno;                                    //将错误码作为返回值返回，从而让父进程得到返回信息
    }
    else
    {
        printf("malloc success\n");
    }

    return ret;
}
