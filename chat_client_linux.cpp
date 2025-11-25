/**
 * @file chat_client.cpp
 * @brief Improved Linux TCP chat client for OOPS project.
 */

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

std::mutex coutMutex;

/**
 * @brief Handles receiving messages from the server.
 */
void receiveMessages(int socketFd) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\n" << buffer << std::endl;
            std::cout << "You: ";
        } 
        else {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\nDisconnected from server." << std::endl;
            break;
        }
    }
}

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Server details
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to the server." << std::endl;
        return 1;
    }

    std::cout << "Connected to server." << std::endl;

    // LOGIN flow
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);

    std::string loginCmd = "LOGIN " + username;
    send(clientSocket, loginCmd.c_str(), loginCmd.size(), 0);

    // Start receiver thread
    std::thread receiver(receiveMessages, clientSocket);

    // Sending loop
    std::string message;
    std::cout << "You: ";
    while (true) {
        std::getline(std::cin, message);

        if (message == "/quit") {
            send(clientSocket, message.c_str(), message.size(), 0);
            break;
        }

        ssize_t sent = send(clientSocket, message.c_str(), message.size(), 0);
        if (sent <= 0) {
            std::cout << "\nFailed to send message. Server may be offline." << std::endl;
            break;
        }
    }

    close(clientSocket);
    receiver.join();

    return 0;
}
