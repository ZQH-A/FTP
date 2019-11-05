#include "transport.h"

Transport::Transport()
{
    path="/";
}

void Transport::T_Menu(int socketfd)//发送目录
{
    DIR *dr;
    struct dirent *dir;
    if((dr=opendir(path.data()))==NULL)
        printf("error");
    while((dir=readdir(dr))!=NULL)
    {
        if(strncmp(dir->d_name,".",1)==0)
        {
            if(strncmp(dir->d_name,"..",2)==0)
            {                

            }else {
                continue;
            }
        }

        bzero(context,M_SIZE);

        Json::Value rep;
        Json::StreamWriterBuilder writerBuilder;
        std::ostringstream os;
        rep["filename"]=dir->d_name;
        rep["type"]=dir->d_type;

        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(rep, &os);
        std::string jsonStr;
        jsonStr = os.str();

        strcat(context,jsonStr.data());
        SdHeader(socketfd,1,strlen(context));
        writen(socketfd,strlen(context));
    }
    SdHeader(socketfd,0);
    type=0;
}

void Transport::Recv_f(int socketfd,int length,std::string path)//接收文件
{
    readn(socketfd,length);
    std::string name=context;
    std::string textname=path+name;

    FILE *fp;
    if((fp=fopen(textname.data(),"w+"))==NULL)
        err_exit("error");
    while (1){
        int n=RvHeader(socketfd);
        if (type==0)
            break;
        if(type==1)
        {
            readn(socketfd,n);
            fwrite(context,sizeof(char),n,fp);
        }
    }
    fclose(fp);
}

void Transport::Recv_D(int socketfd, int length)//接收文件夹
{
    readn(socketfd,length);
    std::string floder=path+context+"/";
    if(mkdir(floder.data(),0755)==-1)
         printf("mkdir error\n");
    while (1) {
        int n=RvHeader(socketfd);
        if(type==8)
            break;
        if(type==5)
            Recv_f(socketfd,n,floder);
        if(type==6){
            readn(socketfd,n);
            floder=floder+context+"/";
            if(mkdir(floder.data(),0755)==-1)
                 printf("mkdir error\n");
        }
        if(type==7)
            floder+="../";
    }
}

void Transport::cd(int socketfd,int length)//进入目录
{
    readn(socketfd,length);
    std::string name=context;
    path=path+name+"/";
    type=0;
    T_Menu(socketfd);
}

void Transport::flush(int socketfd)//刷新
{
    T_Menu(socketfd);
    type=0;
}

ssize_t Transport::readn(int socketfd,size_t length)//读取一定的字节
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

ssize_t Transport::writen(int socketfd, size_t length)//写一定的字节
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

int Transport::SdHeader(int socketfd,int type, int length)//发送头部和长度
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
    }
    int c=strlen(s);
    int n=write(socketfd,s,c);
    if (n==14 || n==4)
        return 1;
    return -1;
}

int Transport::RvHeader(int socketfd)//接收头部和长度
{
    int n;
    bzero(context,M_SIZE);

    n=recv(socketfd,context,4,0);
    if (n<4)
        return -1;
    uint16_t i=atoi(context);
    int c=ntohs(i);
    if (c==DATAFin|| c==GET)
    {
        type=0;
        return 0;
    }
    if (c== CD){
        type=2;
    }
    if (c==FLUSH){
        type=4;
        return 0;
    }
    if(c== MESSAGB)
    {
        type=1;
    }
    if(c==UPLOAD)
    {
        type=5;
    }
    if(c==UPLOAD_D)
    {
        type=6;
    }
    if (c==DOCKFIN)
    {
        type=7;
        return 0;
    }
    if(c==T_DOCKFIN)
    {
        type=8;
        return 0;
    }
    n=recv(socketfd,context,15,0);
    if (n<10)
        return -1;
    uint32_t d=atoi(context);
    c=ntohl(d);
    return c;
}

int Transport::Type()//返回类型
{
    return type;
}

std::string Transport::Path()
{
    return path;
}
