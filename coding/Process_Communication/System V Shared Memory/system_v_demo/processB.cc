#include "comm.hpp"

int main()
{
    int shmid = get_shm();                                    // 获取共享内存ID
    char *shmaddr = (char*)shmat(shmid, nullptr, 0);          // 挂接共享内存

    int fd = open(FIFO_FILE, O_WRONLY);                       // 打开FIFO文件
    if(fd < 0)
    {
        perror("打开文件失败！");
        log(Error, "打开文件失败！");
        exit(FIFO_OPEN_ERR);
    }

    while(true)
    {
        cout << "请输入要发送的消息：";
        fgets(shmaddr, 4096, stdin);                          // 从标准输入读取消息
        write(fd, "c", 1);                                    // 发送消息
    }

    shmdt(shmaddr);                                           // 脱挂共享内存
    close(fd);                                                // 关闭FIFO文件

    return 0;
}