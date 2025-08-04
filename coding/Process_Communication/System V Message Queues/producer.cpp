#include "comm.hpp"

int main()
{
    int key_t = key;
    int msgid = msgget(key_t, IPC_CREAT | 0666);
    if(msgid < 0)
    {
        perror("msgget failed");
        exit(1);
    }

    my_msgbuf msg;
    msg.mtype = 1;
    strcpy(msg.mtext, "Hello, linux!");

    if(msgsnd(msgid, &msg, sizeof(msg), IPC_NOWAIT) < 0)
    {
        perror("msgsnd failed");
        exit(1);
    }
    
    cout << "消息发送成功！" << endl;

    return 0;
}