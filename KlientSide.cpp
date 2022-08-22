#include "KlientSide.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <thread>
#include <mutex>
#include <chrono>

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>


void KlientSide::dataProcessing(const std::string& str, std::string& res){
    int res_i = 0;
    for(const char a: str){
       if(a >= '0' && a <= '9'){
            char s[2];
            strncpy(s, &a, 1);
            s[1] = '\0';
            int ds = atoi(s);
            res_i += ds;
       } 
    }
    char d[10] = {};
    sprintf(d, "%d", res_i);
    res = std::string(d);
}
KlientSide::KlientSide(const std::string& socket_name){
    strncpy(this->socket_name, socket_name.c_str(), 108);
    load_socket();
}
KlientSide::~KlientSide(){
    std::cout << "This is the end!" << std::endl;
}

void KlientSide::secondThread(std::mutex& first_lock){
    std::vector<std::string>data_mas;
    while(true){
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    first_lock.lock();
        char data[130];
        memset(data, 0, 130);
        strcpy(data, common_bufer);
        memset(common_bufer, 0, 130);
        std::cout << "Ready data to send:\t" << data << std::endl;
    first_lock.unlock();
        std::string numb;
        dataProcessing(std::string(data), numb);
        strcpy(data, numb.c_str());       
        if(send(sock, data, 0, MSG_NOSIGNAL) == -1){
            data_mas.push_back(std::string(data));
            if(!load_socket()){
                continue;
            }
        }
        if(data_mas.empty()){
            if(data[0] != '\0'){
                const char sd = '\0';
                strcat(data, &sd);
                int er = send(sock, data, sizeof(data), MSG_NOSIGNAL);
                if(er == -1) {
                    data_mas.push_back(std::string(data));
                    continue;
                }
                else {
                    data[0] = '\0';
                    continue;
                }
            }
        }
        else{
            int i = 0;
            for(auto s: data_mas){
                memset(data, 0, 130);
                strncpy(data, s.c_str(), s.length());
                const char sd = '\0';
                strcat(data, &sd);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                int er = send(sock, data, sizeof(data), MSG_NOSIGNAL);
                if(er == -1) {
                    break;
                }
                i++;          
            }
            for(int ii = 0; ii < i; ii++){
                data_mas.erase(data_mas.begin());
            }
        }
    }
}

void KlientSide::firstThread(std::mutex& first_lock){
    first_lock.lock();
    dataEntering();
    for(int i = 0; is_invalid(); i++){
        if(i != 0){    
            std::cout << "You have entered data that is not digits or > 64. Try again." << std::endl;
            dataEntering();
        }
    }
    bubbleSort();
    char temp[130] = {};
    for(int i = 0; i < strlen(common_bufer); i++){
        const char a = common_bufer[i];
        if(a=='0'||a=='2'||a=='4'||a=='6'||a=='8'){
            strcat(temp, "KB");
        }
        else{
            strncat(temp, &a, 1);
        }
    }
    strcpy(common_bufer, temp);
    const char sd = '\0';
    strcat(common_bufer, &sd);
    first_lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}



void KlientSide::dataEntering(){
        std::cout << "\nEnter the numeric data. (Numbers only! And < 64 chapters)" << std::endl;
        std::cin >> common_bufer;
        std::cout << '\n';
}

void KlientSide::bubbleSort()
{
    char tmp = 0;
    for(int i = 0;i<strlen(common_bufer);i++){
        for(int j = (strlen(common_bufer)-1);j>=(i+1);j--){
            if(common_bufer[j]>common_bufer[j-1]){
                tmp = common_bufer[j];
                common_bufer[j]=common_bufer[j-1];
                common_bufer[j-1]=tmp;
            }
        }
    }
}

bool KlientSide::is_invalid(){
        bool invalid = false;
        if(strlen(common_bufer)<=64){
            for(int i = 0; i < strlen(common_bufer); i++){
                if(common_bufer[i] < '0' || common_bufer[i] > '9'){
                    return true;
                }
            }
            return false;
        }
        return true;
}

bool KlientSide::load_socket(){
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
    }
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_name, sizeof(addr.sun_path) - 1);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    return 1;
}
