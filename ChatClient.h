#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUF_SIZE 512

class ChatClient {
public:
    ChatClient(const std::string& username);
    void run();

private:
    WSAData wsa;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    std::string username;

    void receiveMessages();
    
};