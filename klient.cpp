#include "KlientSide.hpp"

#include <thread>
#include <mutex>
#include <string>

int main()
{
    std::mutex first_lock;
    KlientSide klient(std::string("/tmp/fileServer1.socet"));
    std::thread t1([&first_lock, &klient](){while(1)klient.firstThread(first_lock);});
    std::thread t2([&first_lock, &klient](){klient.secondThread(first_lock);});
    t1.join();
    t2.join();
    return 0;
}
