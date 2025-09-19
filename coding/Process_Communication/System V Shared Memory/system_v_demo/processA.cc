#include "comm.hpp"

int main()
{
    int shmid = create_shm_only();                            // 创建共享内存（只创建）
    char *shmaddr = (char *)shmat(shmid, nullptr, 0);         // 挂接共享内存

    int fd = open(FIFO_FILE, O_RDONLY);                       // 打开管道文件
    if(fd < 0)
    {
        perror("打开文件失败！");
        log(Error, "打开文件失败！");
        exit(FIFO_OPEN_ERR);
    }

    struct shmid_ds shmds;                                    // 共享内存信息结构体

    while(true)
    {
        char c;                                               // 读取字符
        ssize_t s = read(fd, &c, 1);                          // 读取管道文件
        if(s < 0 || s == 0)
        {
            break;
        }

        cout << "客户说：" << shmaddr << endl;
        sleep(1);                                             // 模拟业务处理时间，延迟1秒

        shmctl(shmid, IPC_STAT, &shmds);                      // 获取共享内存信息
        cout << "段大小: " << shmds.shm_segsz << endl;
        cout << "附着进程数: " << shmds.shm_nattch << endl;
        printf("传递给进程的键值: 0x%x\n",  shmds.shm_perm.__key);
        cout << "访问模式: " << shmds.shm_perm.mode << endl;
    }

    shmdt(shmaddr);                                           // 脱挂共享内存
    shmctl(shmid, IPC_RMID, nullptr);                         // 删除共享内存
    close(fd);                                                // 关闭文件描述符

    return 0;
}