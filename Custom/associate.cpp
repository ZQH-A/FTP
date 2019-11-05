#include "associate.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <iostream>
#include "err_exit.h"
#include <dirent.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

Associate::Associate()
{    
}

bool Associate::open(std::string IP)//连接服务器
{
    socketfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in name;
    name.sin_family=AF_INET;
    name.sin_port=htons(9001);
    const char *s=IP.c_str();
    inet_aton(s,&name.sin_addr);

    int error=-1, len;
    len = sizeof(int);
    timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(socketfd, FIONBIO, &ul); //设置为非阻塞模式

    if( connect(socketfd, (struct sockaddr *)&name, sizeof(name)) == -1)
    {
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        ul = 0;
        FD_SET(socketfd, &set);
        if( select(socketfd+1, NULL, &set, NULL, &tm) > 0)
        {
            getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error,      (socklen_t *)&len);
            if(error == 0){
                ioctl(socketfd, FIONBIO, &ul);
                return true;
            }
            else{
                ioctl(socketfd, FIONBIO, &ul);
                close(socketfd);
                return false;
            }
        }else{
            ioctl(socketfd, FIONBIO, &ul);
            close(socketfd);
            return false;
        }
    }
    return true;
}

void Associate::RecvMenu()//接收目录写入一个文件
{
    FILE *fp;
    if((fp=fopen("./Menu","w+"))==NULL)
        err_exit("file");
    char Menu[SIZE];
    while(1)
    {
        bzero(Menu,SIZE);
        int length=RvHeader();
        if (length==0 || length==-1)
            break;
        length=readn(length);
        if (length == -1)
            break;
        std::string mge(context,strlen(context));

        Json::Value rep;

        bool res;
        JSONCPP_STRING errs;
        Json::CharReaderBuilder readerBuilder;

        std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
        res = jsonReader->parse(mge.c_str(), mge.c_str()+mge.length(), &rep, &errs);
        if (!res || !errs.empty()) {
            std::cout << "parseJson err. " << errs << std::endl;
        }

        std::string filename=rep["filename"].asString();
        std::string type=rep["type"].asString();

        std::string name=filename+" "+type;
        strcpy(Menu,name.c_str());

        int n=strlen(Menu);
        Menu[n]='\n';
        fwrite(Menu,sizeof(char),strlen(Menu),fp);
    }
    fclose(fp);
}

int Associate::SdHeader(int type,int length)//发送文件头部和文件的长度
{
    char s[50];
    char a[20];
    bzero(s,50);
    bzero(a,20);
    if (type==1)
    {
        sprintf(s,"%4d",htons(MESSAGB));
        sprintf(a,"%15d",htonl(length));
        strcat(s,a);
    }else if (type ==0) {
        sprintf(s,"%4d",htons(DATAFin));
    }else if(type==2)
    {
        sprintf(s,"%4d",htons(CD));
        sprintf(a,"%15d",htonl(length));
        strcat(s,a);
    }else if(type==3)
    {
        sprintf(s,"%4d",htons(GET));
    }else if(type == 4)
    {
        sprintf(s,"%4d",htons(FLUSH));
    }else if (type==5) {
        sprintf(s,"%4d",htons(UPLOAD));
        sprintf(a,"%15d",htonl(length));
        strcat(s,a);
    }else if(type==6)
    {
        sprintf(s,"%4d",htons(UPLOAD_D));
        sprintf(a,"%15d",htonl(length));
        strcat(s,a);
    }else if (type==7) {
        sprintf(s,"%4d",htons(DOCKFIN));
    }else if(type==8)
    {
        sprintf(s,"%4d",htons(T_DOCKFIN));
    }
    int n=write(socketfd,s,strlen(s));

    if (n==14 || n==4)
        return 1;
    return -1;
}

int Associate::RvHeader()//接收头部并返回要接收的长度
{
    int n;
    bzero(context,M_SIZE);

    n=recv(socketfd,context,4,0);
    if (n<4)
        return -1;
    uint16_t i=atoi(context);
    int c=ntohs(i);
    if (c==DATAFin)
        return 0;
    n=recv(socketfd,context,15,0);
    if (n<10)
        return -1;
    uint32_t d=atoi(context);

    c=ntohl(d);
    return c;
}

void Associate::T_path(std::string path)//传送文件名
{
    SdHeader(2,path.length());
    bzero(context,M_SIZE);
    strcat(context,path.data());
    writen(strlen(context));
}

void Associate::UpLoad_F(std::string path,std::string textname)//上传文件
{
    FILE *fp;
    std::string CpxPath=path+textname;

    SdHeader(5,textname.length());
    bzero(context,M_SIZE);

    strcat(context,textname.data());
    writen(strlen(textname.data()));//文件名称
    if((fp=fopen(CpxPath.data(),"r+"))==NULL)
        err_exit("error");
    bzero(context,M_SIZE);
    int n;

    while ((n=fread(context,sizeof(char),M_SIZE,fp))>0) {//文件内容
        SdHeader(1,n);
        writen(n);
        bzero(context,M_SIZE);
    }
    SdHeader(0);
    fclose(fp);
}

void Associate::UpLoad_D(std::string path, std::string textname)//上传文件夹
{
    AppTo_D(path,textname,1);
}

void Associate::AppTo_D(std::string path,std::string name,int sig)//文件夹
{
    std::string folder=path+name;
    DIR *dr;
    struct dirent *dir;
    if((dr=opendir(folder.c_str()))==NULL)
        printf("error");
    //发送文件夹名称
    SdHeader(6,name.length());
    strcpy(context,name.data());
    writen(name.length());

    while((dir=readdir(dr))!=NULL)
    {
        std::string s=dir->d_name;
        if (dir->d_type==4)//子文件夹
        {
            if (strcmp(dir->d_name,"..")==0)
                continue;
            if (strcmp(dir->d_name,".")==0)
                continue;
            std::string Docufile=folder+"/";
            AppTo_D(Docufile,dir->d_name,0);
            SdHeader(7);
        }else if (dir->d_type==8)//文件
        {
            std::string filepath=folder+"/";
            UpLoad_F(filepath,dir->d_name);
        }
    }
    if(sig==1){
        SdHeader(8);
    }
}

void Associate::DownLoad()//下载
{
}

int Associate::Flush_Sever()//刷新
{
    SdHeader(4);
}

ssize_t Associate::readn(size_t length)//读取n个字节
{
    size_t nleft;
    ssize_t nread;
    char *ptr;
    bzero(context,M_SIZE);
    ptr=context;
    nleft=length;
    while(nleft>0)
    {
        if((nread=read(socketfd,ptr,nleft))<0)
        {
            if (errno==EINTR)
                nread=0;
            else {
                return -1;
            }
        }else if(nread==0)
        {
            break;
        }
        nleft-=nread;
        ptr+=nread;
    }
    return (length-nleft);
}

ssize_t Associate::writen(size_t length)//写入n个字节
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr=context;
    nleft=length;
    while (nleft>0) {
        if((nwritten=write(socketfd,ptr,nleft))<=0)
        {
            if(nwritten<0 && errno == EINTR)
                nwritten=0;
            else
                return -1;
        }
        nleft -=nwritten;
        ptr += nwritten;
    }
    return length;
}


