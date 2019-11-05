#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <thread>
class MyThread
{
public:

    MyThread();
    ~MyThread();
    static void server(int sockfd);
    void start(int sockfd);
private:    
};
#endif // MYTHREAD_H
