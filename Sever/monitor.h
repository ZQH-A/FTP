#ifndef MONITOR_H
#define MONITOR_H
#include <thread>
#include <mutex>
#include <vector>


class Monitor
{
private:
    Monitor(){}
private:

    bool m_live;
    std::vector<std::thread> m_thread;
    std::mutex m_mutex;

public:    
    static Monitor*GetInstance();
    int startService();
    void stopService();
    void monitor(std::thread mythread);
    void service();
    class B
    {
    public:
        ~B();
    };
};
//Monitor *Monitor::m_instance=NULL;
#endif // MONITOR_H
