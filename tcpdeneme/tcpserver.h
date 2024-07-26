#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

class TCPServer {
public:
    TCPServer(const std::string& ipAddress = "127.0.0.1", int port = 12345);
    ~TCPServer();

    void start();
    void stop();

private:
    void initializeWinsock();
    void createSocket();
    void bindSocket();
    void run();
    void handleClient(SOCKET clientSocket);

    std::string ipAddress;
    int port;
    SOCKET listenSocket;
    std::vector<std::thread> clientThreads;
    std::mutex mtx;
    bool stopFlag;
};

#endif // TCPSERVER_H
