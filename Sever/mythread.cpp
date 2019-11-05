#include "mythread.h"
#include <sys/socket.h>
#include <stdio.h>
#include <thread>
#include "monitor.h"
#include <arpa/inet.h>
#include "transport.h"
#include <string.h>

MyThread::MyThread()
{
}

MyThread::~MyThread()
{
}

void MyThread::server(int sockfd)
{
    Transport tpt;
    tpt.T_Menu(sockfd);

    while (1) {
        int length=tpt.RvHeader(sockfd);
        int type=tpt.Type();
        if (type==2)
        {            
            tpt.cd(sockfd,length);
        }else if (type==4) {
            tpt.flush(sockfd);
        }else if(type==5)
        {
            tpt.Recv_f(sockfd,length,tpt.Path());
        }else if(type==6)
        {
            tpt.Recv_D(sockfd,length);
        }
    }
}

void MyThread::start(int sockfd)
{
    Monitor *A= Monitor::GetInstance();
    A->monitor(std::thread(server,sockfd));
}

