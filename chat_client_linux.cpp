/**
 * @file chat_client.cpp
 * @brief A Linux-compatible C++ frontend for the TCP chat application.
 * @author sabyasachiGorai
 * @date Novemember 21 2025
 * Works inside GitHub Codespaces or any Linux environment.
 */

#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
 * @brief Handles receiving messages from the server.
 * @param socketFd The connected socket.
 */
void receiveMessages(int socketFd) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << buffer << std::endl;
        } else if (bytesReceived == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        } else {
            std::cerr << "Error receiving data." << std::endl;
            break;
        }
    }
}

int main() {

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4000); 
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    // Start receiver thread
    std::thread receiver(receiveMessages, clientSocket);

    // Main loop for sending messages
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/quit") {
            break;
        }
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    // Cleanup
    close(clientSocket);
    receiver.join();
    return 0;
}
