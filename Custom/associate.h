#ifndef ASSOCIATE_H
#define ASSOCIATE_H

#include <string.h>
#include "./json/json.h"

#define SIZE 1024
#define M_SIZE 1024*24

#define MESSAGB 0x0103//文件传输
#define DATAFin 0x0104//文件传输完毕
#define GET     0x0105
#define CD      0x0106//进入文件夹
#define FLUSH   0x0107//刷新
#define UPLOAD  0x0108//上传文件
#define UPLOAD_D 0x0109//文件夹
#define DOCKFIN  0x0110//子文件夹结束
#define T_DOCKFIN 0x0111//文件夹结束
class Associate
{
public:
    Associate();
    bool open(std::string IP);//网络连接
    void RecvMenu();//接收服务器的文件列表，保存到一个文件中
    int SdHeader(int type,int length=0);//发送文件头和文件大小
    int RvHeader();
    void T_path(std::string path);//点击服务器文件列表，进入该列表，发送进入文件的名称
    void UpLoad_F(std::string path,std::string textname);//文件发送
    void UpLoad_D(std::string path,std::string textname);//文件夹发送
    void AppTo_D(std::string path,std::string name,int sig);
    void DownLoad();
    int Flush_Sever();
    ssize_t readn(size_t length);
    ssize_t writen(size_t length);
private:
    int socketfd;
    char buf[SIZE];//
    char context[M_SIZE];
};

#endif // ASSOCIATE_H
