#include "ChatClient.h"

ChatClient::ChatClient(const std::string& username) : username(username) {
    WSAStartup(MAKEWORD(2, 2), &wsa);
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    int iResult = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Connection to server failed");
    }

    send(clientSocket, username.c_str(), username.length(), 0);

    char response[MAX_MESSAGE_LENGTH];
    int bytesSize = recv(clientSocket, response, MAX_MESSAGE_LENGTH, 0);
    if (bytesSize <= 0) {
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Failed to receive response from the server");
    }

    response[bytesSize] = '\0';
    if (std::string(response) == "Username is already taken") {
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Username is already taken");
    }

    std::cout << response << std::endl;

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
        if (message.length() > MAX_MESSAGE_LENGTH) {
            std::cout << "Message is too long. Maximum length is " 
                << MAX_MESSAGE_LENGTH << " characters." << std::endl;
            continue;
        }
        std::cout << "You: " << message << std::endl;
        std::string fullMessage = username + ": " + message;
        send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
    }
}

void ChatClient::receiveMessages() {
    char buffer[MAX_MESSAGE_LENGTH];

    while (true) {
        int bytesSize = recv(clientSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        if (bytesSize <= 0) break;
        buffer[bytesSize] = '\0';
        std::cout << buffer << std::endl;
    }
}