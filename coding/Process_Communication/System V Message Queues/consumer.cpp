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
    msgrcv(msgid, &msg, sizeof(msg), 1, 0);
    cout << "消息类型：" << msg.mtype << endl;
    cout << "消息内容：" << msg.mtext << endl;

    msgctl(msgid, IPC_RMID, NULL);
    cout << "队列已删除" << endl;

    return 0;
}