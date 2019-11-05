#include "monitor.h"
#include <unistd.h>
Monitor *Monitor::GetInstance()
{
    static Monitor *m_instance;
    if(m_instance==NULL)
    {
        m_instance=new Monitor();
    }
    return m_instance;
}

int Monitor::startService()
{
    m_live=false;
    return 0;
}

void Monitor::stopService()
{
    m_live=true;
}

void Monitor::monitor(std::thread mythread)
{
    m_mutex.lock();
    m_thread.push_back(std::move(mythread));
    m_mutex.unlock();
}

void Monitor::service()
{
    while(!m_live){
        std::vector<int> a;
        auto iter1=a.begin();
        for(auto iter=m_thread.begin();iter!=m_thread.end();iter++)
        {
            if(iter!=m_thread.end()&&iter1!=a.end()){
                if(*iter1==1)
                {
                    iter++;
                    iter1++;
                }else if(iter->joinable())
                {
                    iter->join();
                    a.push_back(1);
                    iter1++;
                }
            }
        }
        sleep(1);
    }
}

Monitor::B::~B()
{
}
