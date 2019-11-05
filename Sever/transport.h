#ifndef TRANSPORT_H
#define TRANSPORT_H
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include "json/json.h"
#include <dirent.h>
#include <unistd.h>
#include "err_exit.h"
#include <sys/stat.h>

#define MESSAGB 0x0103//文件传输
#define DATAFin 0x0104//文件传输完毕
#define GET     0x0105
#define CD      0x0106
#define FLUSH   0x0107
#define UPLOAD  0x0108
#define UPLOAD_D 0x0109//文件夹
#define DOCKFIN  0x0110//子文件夹结束
#define T_DOCKFIN 0x0111//文件夹结束
#define M_SIZE 24*1024
#define N_SIZE 1024

class Transport
{
public:
    Transport();
    void T_Menu(int socketfd);//发送文件列表
    void Recv_f(int socketfd,int length,std::string path);//接收文件
    void Recv_D(int socketfd,int length);//接收文件夹
    void cd(int socketfd,int length);//进入目录
    void flush(int socketfd);//刷新
    ssize_t readn(int socketfd,size_t length);//读取一定的字节
    ssize_t writen(int socketfd,size_t length);//写一定的字节
    int SdHeader(int socketfd,int type,int length=0);//发送文件头和文件大小
    int RvHeader(int socketfd);//接收文件头
    int Type();//返回类型
    std::string Path();
private:
    char buf[N_SIZE];
    char context[M_SIZE];
    int type;
    std::string path;
};

#endif // TRANSPORT_H
