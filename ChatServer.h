#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUF_SIZE 512
#define MAX_CONNECTION 5

// Global Variables (client threads can access them)
extern std::vector<SOCKET> clients;
extern std::mutex clientsMutex;

class ChatServer {
public:
    ChatServer();
    void run();
private:
    WSAData wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;

    void handleClient(SOCKET clientSocket);
    void broadcastMessage(const std::string& message, SOCKET senderSocket);
};