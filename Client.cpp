#include "ChatClient.h"

int main() {
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    ChatClient client(username);
    client.run();
    return 0;
}