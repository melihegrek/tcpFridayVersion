#include "TCP.h"
#include "TCPServer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "Config.h"
#include <tinyxml2.h>

void runServer(const std::string& ipAddress, int port) {
    TCPServer server(ipAddress, port);
    server.start();
    std::this_thread::sleep_for(std::chrono::seconds(10)); // Server'ı 10 saniye çalıştır
    server.stop();
}

void runClient(const std::string& ipAddress, int port) {
    TCP& tcpClient = TCP::getInstance();
    tcpClient.setIpAddress(ipAddress);
    tcpClient.setPort(port);
    tcpClient.start();

    // Test verilerini gönder
    for (int i = 0; i < 5; ++i) {
        tcpClient.insert("Message " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    tcpClient.stop();
}

int main() {
    Config& config = Config::getInstance();
    if (!config.loadConfig()) {
        std::cerr << "Configuration load failed. Exiting." << std::endl;
        return -1;
    }

    std::string ipAddress = config.getIpAddress();
    int port = config.getPort();
    std::cout<<"IpAdress:"<<ipAddress<<"\tPort:"<<port<<std::endl;


    std::thread serverThread(runServer, ipAddress, port);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Server'ın başlatılmasını bekle

    runClient(ipAddress, port);

    serverThread.join();
    return 0;
}
