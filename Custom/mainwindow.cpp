#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <unistd.h>
#include <thread>
#include <future>
#include <dirent.h>
#include <iostream>
#include <QMenu>
#include "myitemdrawer.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    _Asoc=new Associate();
    m_path="/";

    status=false;

    ui->listWidget->setViewMode(QListView::ListMode);
    listMenu(m_path);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->textEdit->setReadOnly(true);//设置为只能读,不能写
    MyItemDrawer *drawer=new MyItemDrawer(ui->listWidget);
    ui->listWidget->setItemDelegate(drawer);//设置委托

    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleclicked(QListWidgetItem*)));
    connect(ui->listWidget_2,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleclicked_2(QListWidgetItem*)));
    connect(this,SIGNAL(severlogin()),this,SLOT(ShowMenu()));

    connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(contextMenu(const QPoint&)));
    connect(ui->listWidget_2,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(contextMenu_2(const QPoint&)));

    connect(ui->actionUpLoad,SIGNAL(triggered()),this,SLOT(UpLoad()));
    connect(ui->actionDownLoad,SIGNAL(triggered()),this,SLOT(DownLoad()));
    connect(ui->actionEnter,SIGNAL(triggered()),this,SLOT(EnterMenu()));
    connect(ui->actionEnter_2,SIGNAL(triggered()),this,SLOT(EnterMenu_2()));

    connect(ui->actionReflush,SIGNAL(triggered()),this,SLOT(flush()));
    connect(ui->actionReflush_2,SIGNAL(triggered()),this,SLOT(flush_2()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::listMenu(std::string path)//客户端的文件列表
{
    DIR *dr;
    struct dirent *dir;
    if((dr=opendir(path.c_str()))==NULL)
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
        QListWidgetItem *Item= new QListWidgetItem();
        if(dir->d_type==4)// 4是目录，8是文件
        {
//            Item->setIcon(QIcon("../Custom/icon/folder.ico"));
//            Item->setText(dir->d_name);
//            Item->setData(Qt::UserRole,4);

            std::string filename=path+dir->d_name;//显示目录信息
            char *buf=last_mode_time((char *)filename.data());
            Item->setData(Qt::DisplayRole,dir->d_name);
            Item->setData(Qt::UserRole+2,"目录");
            Item->setData(Qt::UserRole+3,buf);

            ui->listWidget->addItem(Item);
        }else if(dir->d_type==8){

//            Item->setIcon(QIcon("../Custom/icon/document.ico"));
//            Item->setData(Qt::UserRole,8);
//            Item->setText(dir->d_name);

            std::string filename=path+dir->d_name;//显示文件信息
            int n=file_size((char *)filename.data());
            char *buf=last_mode_time((char *)filename.data());
            Item->setData(Qt::DisplayRole,dir->d_name);
            Item->setData(Qt::UserRole+1,n);
            Item->setData(Qt::UserRole+2,"文件");
            Item->setData(Qt::UserRole+3,buf);

            ui->listWidget->addItem(Item);
        }
    }
}

std::string MainWindow::Textname(char buf[])
{
    int n=strlen(buf);
    std::string name=buf;
    int len=20-n;
    std::string space(len,' ');
    name=name+space;
    return name;
}

int MainWindow::file_size(char *filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
    return size;
}

char *MainWindow::last_mode_time(char *filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    char *buf=ctime( &statbuf.st_mtime);
    return buf;
}

void MainWindow::on_pushButton_clicked()//点击连接后连接服务器
{
    QString IP=ui->lineEdit->text();
    if (IP.isEmpty())
    {
        QMessageBox::critical(this,tr("语法错误"),tr("无法解析服务器地址:\n 没有给出一个主机，请输入一个主机。"),QMessageBox::Yes);

    }else{
        ui->textEdit->setReadOnly(false);
        if(_Asoc->open(IP.toStdString())){

            ui->textEdit->setText("状态:	正在连接 ...\n连接成功...");
            status=true;
            emit severlogin();
        }else {
            ui->textEdit->setText("状态:	正在连接 ...\n连接超时...");
            status=false;
        }
        ui->textEdit->setReadOnly(true);
    }
}

void MainWindow::ShowMenu()//连接成功后，显式服务器文件列表
{
    _Asoc->RecvMenu();
    FILE *fp;
    fp=fopen("./Menu","r+");
    char Menu[SIZE];
    while (fgets(Menu,SIZE,fp)) {
        char name[100];
        memset(name,0,100);
        for(int i=0;i<strlen(Menu);i++)
        {
            if(Menu[i]==' ')
                break;
            name[i]=Menu[i];
        }
        name[strlen(name)]='\0';
        char digital[4];
        memset(digital,0,4);
        int n=0;
        for(int i=0;i<strlen(Menu);i++)
        {
            if(Menu[i]>='0' && Menu[i]<='9')
            {
                digital[0]=Menu[i];
                n++;
            }
        }
        digital[1]='\0';
        QListWidgetItem *Item= new QListWidgetItem();
        if(digital[0]=='4')
        {
            Item->setIcon(QIcon("../Custom/icon/folder.ico"));
            Item->setText(name);
            Item->setData(Qt::UserRole,4);
            ui->listWidget_2->addItem(Item);
        }else if(digital[0]=='8'){
            Item->setIcon(QIcon("../Custom/icon/document.ico"));
             Item->setData(Qt::UserRole,8);
            Item->setText(name);
            ui->listWidget_2->addItem(Item);
        }
    }
}

void MainWindow::doubleclicked(QListWidgetItem* Item)
{
    if (Item->data(Qt::DisplayRole)!=".."&& Item->data(Qt::UserRole+2)=="目录")
    {
        QString text0=Item->data(Qt::DisplayRole).toString();
        std::string text=text0.toStdString();
        m_path=m_path+text+"/";
        ui->listWidget->clear();
        listMenu(m_path);
    }else if (Item->data(Qt::DisplayRole)=="..") {
        char path[100];
        strcpy(path,m_path.c_str());
        int n=strlen(path);
        int i=0;
        if (n>1){
            while (1) {
                if (path[n-1]=='/')
                {
                    i+=1;
                }
                if (i==2)
                    break;
                path[n-1]='\0';
                n--;
            }
        }
        m_path=path;
        ui->listWidget->clear();
        listMenu(m_path);
    }
}

void MainWindow::doubleclicked_2(QListWidgetItem *Item)
{
    std::string path=Item->text().toStdString();
    int n=Item->data(Qt::UserRole).toInt();
    if(n==4){
        _Asoc->T_path(path);
        ui->listWidget_2->clear();
        ShowMenu();
    }
}

void MainWindow::contextMenu(const QPoint &poit)
{
    QMenu *menu=new QMenu();
    if (!status)
    {
        ui->actionUpLoad->setEnabled(status);
    }
    menu->addAction(ui->actionUpLoad);
    menu->addAction(ui->actionEnter);
    menu->addAction(ui->actionReflush);
    menu->exec(ui->listWidget->mapToGlobal(poit));
}

void MainWindow::contextMenu_2(const QPoint &poit)
{
    QMenu *menu=new QMenu();
    if (!status)
    {
        ui->actionDownLoad->setEnabled(status);
        ui->actionEnter_2->setEnabled(status);
        ui->actionReflush_2->setEnabled(status);
    }
    menu->addAction(ui->actionDownLoad);
    menu->addAction(ui->actionEnter_2);
    menu->addAction(ui->actionReflush_2);
    menu->exec(ui->listWidget_2->mapToGlobal(poit));
}

void MainWindow::UpLoad()
{
    QListWidgetItem *Item=ui->listWidget->currentItem();
    if(Item->data(Qt::UserRole)==4)//文件夹
    {
        _Asoc->UpLoad_D(m_path,Item->text().toStdString());
    }else if(Item->data(Qt::UserRole)==8)//文件
    {
        _Asoc->UpLoad_F(m_path,Item->text().toStdString());
    }
    flush_2();
}

void MainWindow::DownLoad()
{

}

void MainWindow::EnterMenu()
{
    QListWidgetItem *Item=ui->listWidget->currentItem();
    doubleclicked(Item);
}

void MainWindow::EnterMenu_2()
{
    QListWidgetItem *Item=ui->listWidget_2->currentItem();
    doubleclicked_2(Item);
}

void MainWindow::flush()
{
    ui->listWidget->clear();
    listMenu(m_path);
}

void MainWindow::flush_2()
{
    _Asoc->Flush_Sever();
    ui->listWidget_2->clear();
    ShowMenu();
}
