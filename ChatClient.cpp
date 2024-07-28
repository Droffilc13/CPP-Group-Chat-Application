#include "ChatClient.h"

ChatClient::ChatClient(const std::string& username) : username(username) {
    WSAStartup(MAKEWORD(2, 2), &wsa);
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    // Separate thread so that sending and receiving messages are on different threads
    std::thread(&ChatClient::receiveMessages, this).detach();
}

void ChatClient::run() {
    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (message == "quit") {
            closesocket(clientSocket);
            break;
        }
        std::string fullMessage = username + ": " + message;
        send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
    }
}

void ChatClient::receiveMessages() {
    char buffer[BUF_SIZE];

    while (true) {
        int bytesSize = recv(clientSocket, buffer, BUF_SIZE, 0);
        if (bytesSize <= 0) break;
        buffer[bytesSize] = '\0';
        std::cout << buffer << std::endl;
    }
}