#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mythread.h"
#include "err_exit.h"
#include <vector>
#include "monitor.h"
using namespace std;
#define LISTENQ 10
void doit(int sockfd)
{
    char buf[255];
    char buff[255]="Yes,fine.";
    int n;
    n=read(sockfd,buf,sizeof (buf));
    buf[n]=0;
//    sleep(1);
    printf("%s\n",buf);
    write(sockfd,buff,n);
}
//vector <thread> mythread;

int main()
{
    int sockfd,client_socket,len;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in name,client_address;
    name.sin_family=AF_INET;
    name.sin_port=htons(9001);

    inet_aton("127.0.0.1",&name.sin_addr);
    if(bind(sockfd,(struct sockaddr*)&name,(socklen_t) sizeof (name))<0)
        err_exit("bind:");
    listen(sockfd,LISTENQ);
    len=sizeof(client_address);
//    while(1)
//    {
//        printf("waiting\n");
//        client_socket=accept(sockfd,(struct sockaddr*)&client_address,(socklen_t*)&len);
//        doit(client_socket);
//        close(client_socket);
//    }

    Monitor *A= Monitor::GetInstance();
    A->startService();
    thread mythread(&Monitor::service,A);
    while(1){
        printf("waiting\n");
        client_socket=accept(sockfd,(struct sockaddr*)&client_address,(socklen_t*)&len);
        MyThread *server=new MyThread();
        server->start(client_socket);
//        thread mythread(doit,client_socket);
//        mythread.join();



//        mythread.push_back(thread(doit,client_socket));
//        printf("%d",mythread.size());


    }
//    for(auto iter=mythread.begin();iter!=mythread.end();iter++)
//    {

//        iter->join();
//    }
    mythread.join();
    return 0;
}
