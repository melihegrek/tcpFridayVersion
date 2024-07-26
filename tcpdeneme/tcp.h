#ifndef TCP_H
#define TCP_H

#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

class TCP {
public:
    struct TCPMessage {
        std::string data;
        std::string dateTime;
    };

    static TCP& getInstance();
    void insert(const std::string& data);
    void start(); // IP ve port ekleme
    void stop();

        // Yeni setter metotları
    void setIpAddress(const std::string& ipAddress);
    void setPort(int port);

    // Disable copy and assignment
    TCP(const TCP&) = delete;
    TCP& operator=(const TCP&) = delete;

private:
    TCP();
    ~TCP();

    void processQueue();
    std::string getCurrentDateTime();
    void initializeWinsock();
    void createSocket();
    void connectSocket();

    std::queue<TCPMessage> messageQueue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopFlag;
    std::thread workerThread;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::string ipAddress; // IP adresi
    int port; // Port numarası
}; 

// TCPMessage yerine vector<char> olsun

#endif // TCP_H
