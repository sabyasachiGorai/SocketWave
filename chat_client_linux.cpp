/**
 * @file chat_client.cpp
 * @brief Improved Linux TCP chat client with colors and timestamps.
 */

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

std::mutex coutMutex;

// ====== COLOR CODES (FEATURE 1) ======
#define RESET   "\033[0m"
#define CYAN    "\033[36m"   // Messages from others
#define GREEN   "\033[32m"   // Server system messages
#define YELLOW  "\033[33m"   // Your own message prefix

// ====== TIMESTAMP FUNCTION (FEATURE 2) ======
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm *tmPtr = std::localtime(&t);

    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%H:%M", tmPtr);
    return std::string(buffer);
}

/**
 * @brief Handles receiving messages from the server.
 */
void receiveMessages(int socketFd) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            std::string msg(buffer);
            std::string ts = "[" + getTimestamp() + "] ";

            std::lock_guard<std::mutex> lock(coutMutex);

            // Color rules
            if (msg.rfind("SERVER:", 0) == 0) {
                std::cout << "\n" << ts << GREEN << msg << RESET << std::endl;
            } else {
                std::cout << "\n" << ts << CYAN << msg << RESET << std::endl;
            }

            std::cout << YELLOW << "You: " << RESET;
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

    std::cout << GREEN << "Connected to server." << RESET << std::endl;

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
    std::cout << YELLOW << "You: " << RESET;

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
