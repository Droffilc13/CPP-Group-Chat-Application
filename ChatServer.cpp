#include "ChatServer.h"

// Initialise here because if initialise in header, there will be multiple import errors
std::vector<SOCKET> clients;
std::mutex clientsMutex;

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

        // Thread safety reasons: Clients sockets can be added or removed, mutex to avoid race conditions
        clientsMutex.lock();
        clients.push_back(clientSocket);
        clientsMutex.unlock();

        std::thread(&ChatServer::handleClient, this, clientSocket).detach();
    }
}

void ChatServer::handleClient(SOCKET clientSocket) {
    char buffer[BUF_SIZE];

    while (true) {
        int bytesSize = recv(clientSocket, buffer, BUF_SIZE, 0);
        if (bytesSize <= 0) {
            closesocket(clientSocket);

            clientsMutex.lock();
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
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