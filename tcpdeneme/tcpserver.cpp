#include "TCPServer.h"
#include <iostream>

TCPServer::TCPServer(const std::string& ipAddress, int port)
    : ipAddress(ipAddress), port(port), listenSocket(INVALID_SOCKET), stopFlag(false) {
    initializeWinsock();
    createSocket();
    bindSocket();
}

TCPServer::~TCPServer() {
    //stop();
    WSACleanup();
}

void TCPServer::initializeWinsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        exit(EXIT_FAILURE);
    }
}

void TCPServer::createSocket() {
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void TCPServer::bindSocket() {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    int iResult = bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void TCPServer::start() {
    int iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    std::thread serverThread(&TCPServer::run, this);
    serverThread.detach();
}

void TCPServer::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopFlag = true;
    }

    // Dinleme soketini kapat
    closesocket(listenSocket);

    // Active client bağlantılarını kapat
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join(); // Thread'lerin bitmesini bekle
        }
    }

    clientThreads.clear(); // Thread listesini temizle

    std::cout << "Server was closed." << std::endl;
}


void TCPServer::run() {
    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (stopFlag) break;
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::lock_guard<std::mutex> lock(mtx);
        clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }

}

void TCPServer::handleClient(SOCKET clientSocket) {
    char buffer[512];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
        if (bytesReceived == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::string message(buffer, bytesReceived);
        std::cout << "Received: " << message << std::endl;

        // Echo the message back to the client
        int sendResult = send(clientSocket, message.c_str(), message.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    closesocket(clientSocket); // Client soketini kapat
}

