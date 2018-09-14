#include "main.h"
#include "threadporc.h"

int main()
{

    pthread_t pRecv, pCmd;      //PID of the thread to receive data and command

    pthread_create(&pRecv, NULL, procRecv, NULL);           //create the thread to receive the data from clients
    pthread_create(&pCmd, NULL, procCmd, (void*)&pRecv);    //创建新线程来接受本机的命令,并将数据接受线程的ID传入,方便控制程序结束

    pthread_join(pRecv, NULL);                              //waiting the thread to exit
    pthread_join(pCmd, NULL);                               //

    return 0;
}
