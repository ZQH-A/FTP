#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "associate.h"
#include <QListWidgetItem>
#include <sys/stat.h>
#include <time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void listMenu(std::string path);
    std::string Textname(char buf[]);
    int file_size(char* filename);//文件大小
    char* last_mode_time(char* filename);//文件最后修改时间
signals:
    void severlogin();
private slots:
    void on_pushButton_clicked();
    void ShowMenu();
    void doubleclicked(QListWidgetItem* Item);
    void doubleclicked_2(QListWidgetItem* Item);
    void contextMenu(const QPoint& poit);
    void contextMenu_2(const QPoint & poit);
    void UpLoad();
    void DownLoad();
    void EnterMenu();
    void EnterMenu_2();
    void flush();
    void flush_2();
private:
    Ui::MainWindow *ui;
    Associate *_Asoc;
    std::string m_path;
    std::string s_path;
    bool status;
};

#endif // MAINWINDOW_H
