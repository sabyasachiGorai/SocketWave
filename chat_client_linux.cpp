/**
 * @file chat_client_linux.cpp
 * @brief A Linux-compatible C++ TCP chat client.
 *
 * This program connects to a server via TCP and allows
 * the user to send and receive messages simultaneously.
 * It works smoothly inside GitHub Codespaces or any Linux machine.
 *
 * Features:
 *  - Creates a TCP socket
 *  - Connects to a server (IP + Port)
 *  - Spawns a thread to constantly listen for incoming messages
 *  - Allows user to type and send messages
 *  - Gracefully handles server disconnect
 *
 * @author sabyasachiGorai
 * @date November 21, 2025
 */

#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
 * @brief Continuously listens for messages from the server.
 *
 * This function runs in a separate thread so the client can
 * both *send* and *receive* messages at the same time.
 *
 * How it works:
 *  - Uses `recv()` to read incoming data from the server.
 *  - If data is received, it's printed to the console.
 *  - If `recv()` returns 0 → server closed connection.
 *  - If `recv()` returns -1 → error occurred.
 *
 * @param socketFd File descriptor of the connected socket.
 */
void receiveMessages(int socketFd) {
    char buffer[1024];

    while (true) {
        // Try to read bytes
        int bytesReceived = recv(socketFd, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            // Null-terminate to create a proper C-string
            buffer[bytesReceived] = '\0';
            std::cout << buffer << std::endl;
        }
        else if (bytesReceived == 0) {
            // Server closed the connection
            std::cout << "Server disconnected." << std::endl;
            break;
        }
        else {
            // Some recv() error happened
            std::cerr << "Error receiving data." << std::endl;
            break;
        }
    }
}

int main() {

    // -----------------------------
    // 1. CREATE THE CLIENT SOCKET
    // -----------------------------
    // AF_INET → IPv4
    // SOCK_STREAM → TCP
    // 0 → automatically pick protocol
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // -----------------------------
    // 2. PREPARE SERVER ADDRESS
    // -----------------------------
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;        // IPv4 format
    serverAddr.sin_port = htons(4000);      // Convert port → network byte order

    // Convert string IP "127.0.0.1" → binary form
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // -----------------------------
    // 3. CONNECT TO THE SERVER
    // -----------------------------
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    // -----------------------------
    // 4. START RECEIVER THREAD
    // -----------------------------
    // This thread keeps listening for messages from server.
    std::thread receiver(receiveMessages, clientSocket);

    // -----------------------------
    // 5. MAIN LOOP — SEND MESSAGES
    // -----------------------------
    std::string message;

    while (true) {
        std::getline(std::cin, message);

        // User can exit using "/quit"
        if (message == "/quit") {
            break;
        }

        // Send the message to the server
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    // -----------------------------
    // 6. CLEAN UP AND EXIT
    // -----------------------------
    // Close socket
    close(clientSocket);

    // Wait for receiver thread to finish
    receiver.join();

    return 0;
}
