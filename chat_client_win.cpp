/**
 * @file chat_client.cpp
 * @brief A Windows-compatible C++ frontend for the chat application.
 *
 * This program connects to a TCP chat server, allows the user to log in,
 * send messages, and receive messages in real-time.
 *
 * @author Vinit
 * @date November 18, 2025
 */

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

/**
 * @brief Initializes Winsock.
 * @return true if initialization is successful, false otherwise.
 */
bool initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Handles receiving messages from the server.
 * @param socket The connected socket.
 */
void receiveMessages(SOCKET socket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);
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
    // Initialize Winsock
    if (!initializeWinsock()) {
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4000); // TCP_PORT from server.js
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    // Start a thread to receive messages
    std::thread receiver(receiveMessages, clientSocket);

    // Main loop to send messages
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/quit") {
            break;
        }
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    receiver.join();
    return 0;
}