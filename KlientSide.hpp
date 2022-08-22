#pragma once
#include <string>
#include <sys/un.h>
#include <mutex>

class KlientSide
{
private:
    int sock;
    char common_bufer[130];
    struct sockaddr_un addr;
    char socket_name[108];
public:
    KlientSide(const std::string& socket_name);
    ~KlientSide();
    
    void firstThread(std::mutex& first_lock);
    void secondThread(std::mutex& first_lock);
private:
    void dataProcessing(const std::string& str, std::string& res);
    bool load_socket();
    bool is_invalid();
    void bubbleSort();
    void dataEntering();
};

