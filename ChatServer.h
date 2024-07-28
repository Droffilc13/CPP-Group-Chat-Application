#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <unordered_set>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUF_SIZE 512
#define MAX_CONNECTION 5
#define USERNAME_ERROR "Username is already taken"

// Global Variables (client threads can access them)
extern std::vector<SOCKET> clients;
extern std::mutex clientsMutex;
extern std::unordered_set<std::string> usernames;

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