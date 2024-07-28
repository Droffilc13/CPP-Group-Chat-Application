#include "ChatServer.h"

// Initialise here because if initialise in header, there will be multiple import errors
std::vector<SOCKET> clients;
std::mutex clientsMutex;
std::unordered_set<std::string> usernames;

ChatServer::ChatServer() {
    WSAStartup(MAKEWORD(2,2), &wsaData);
    serverSocket = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    listen(serverSocket, MAX_CONNECTION);
}

void ChatServer::run() {
    std::cout << "Server started on port " << PORT << std::endl;

    while (true) {
        // accept is blocking call
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::thread(&ChatServer::handleClient, this, clientSocket).detach();
    }
}

void ChatServer::handleClient(SOCKET clientSocket) {
    char buffer[BUF_SIZE];

    int bytesSize = recv(clientSocket, buffer, BUF_SIZE, 0);
    if (bytesSize <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesSize] = '\0';
    std::string username(buffer);

    // Thread safety reasons: Clients sockets can be added or removed, mutex to avoid race conditions
    clientsMutex.lock();
    if (usernames.find(username) != usernames.end()) {
        std::string errorMessage = USERNAME_ERROR;
        send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        closesocket(clientSocket);
        return ;
    } else {
        usernames.insert(username);
        clients.push_back(clientSocket);
        std::string welcomeMessage = "You are connected! Welcome, " + username;
        send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    }
    clientsMutex.unlock();

    while (true) {
        int bytesSize = recv(clientSocket, buffer, BUF_SIZE, 0);
        if (bytesSize <= 0) {
            closesocket(clientSocket);

            clientsMutex.lock();
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            usernames.erase(username);
            clientsMutex.unlock();

            break;
        }
        
        buffer[bytesSize] = '\0';
        broadcastMessage(buffer, clientSocket);
    }
}

void ChatServer::broadcastMessage(const std::string& message, SOCKET senderSocket) {
    clientsMutex.lock();
    for (SOCKET client: clients) {
        if (client != senderSocket) {
            send(client, message.c_str(), message.length(), 0);
        }
    }
    clientsMutex.unlock();
}