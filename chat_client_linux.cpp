/**
 * @file chat_client.cpp
 * @brief Improved Linux TCP chat client with colors and timestamps.
 * * This client uses the socket API to connect to a server (typically 127.0.0.1:4000).
 * It features multi-threading to handle sending and receiving messages concurrently.
 * Enhancements include:
 * 1. ANSI color codes for a better terminal experience.
 * 2. Timestamps for received messages.
 * 3. A simple login flow (sends "LOGIN <username>").
 * 4. A /quit command to gracefully exit.
 */

#include <iostream>
#include <string>
#include <thread>      // For creating the receiver thread
#include <mutex>       // For safe concurrent access to std::cout
#include <chrono>      // For timestamp generation
#include <ctime>       // For timestamp generation (localtime)
#include <iomanip>     // For I/O manipulators (though not strictly used, good for C++ I/O)
#include <unistd.h>    // For close() (used for file descriptors/sockets)
#include <arpa/inet.h> // For inet_pton()
#include <sys/socket.h> // For socket(), connect(), send(), recv()

// Global mutex to protect std::cout from concurrent writes by different threads
std::mutex coutMutex;

// ====== COLOR CODES (FEATURE 1) ======
// ANSI Escape Codes for text coloring in the terminal
#define RESET   "\033[0m"  // Resets color and attributes to default
#define CYAN    "\033[36m"  // Messages from others (e.g., other users)
#define GREEN   "\033[32m"  // Server system messages (e.g., connect/disconnect)
#define YELLOW  "\033[33m"  // Your own message prompt prefix

// ====== TIMESTAMP FUNCTION (FEATURE 2) ======
/**
 * @brief Generates a formatted timestamp string in "HH:MM" format.
 * @return std::string The formatted time string.
 */
std::string getTimestamp() {
    // Get current time point
    auto now = std::chrono::system_clock::now();
    // Convert time point to time_t (C-style time)
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    // Convert time_t to local time structure (tm)
    std::tm *tmPtr = std::localtime(&t);

    char buffer[16];
    // Format the time structure into HH:MM string and store in buffer
    std::strftime(buffer, sizeof(buffer), "%H:%M", tmPtr);
    return std::string(buffer);
}

/**
 * @brief Handles receiving messages from the server.
 * This function runs in a separate thread.
 * It constantly listens for incoming data and prints it to the console
 * with a timestamp and color coding.
 * @param socketFd The file descriptor of the connected socket.
 */
void receiveMessages(int socketFd) {
    char buffer[1024];
    while (true) {
        // Attempt to receive data from the socket. Block until data is available.
        int bytesReceived = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data

            std::string msg(buffer);
            std::string ts = "[" + getTimestamp() + "] "; // Get and format timestamp

            // Lock the mutex to ensure safe output to the console
            std::lock_guard<std::mutex> lock(coutMutex);

            // Color rules: Check if the message starts with "SERVER:"
            if (msg.rfind("SERVER:", 0) == 0) {
                // System message: Use GREEN color
                std::cout << "\n" << ts << GREEN << msg << RESET << std::endl;
            } else {
                // Regular message from another user: Use CYAN color
                std::cout << "\n" << ts << CYAN << msg << RESET << std::endl;
            }

            // Re-print the "You:" prompt after a message is received
            std::cout << YELLOW << "You: " << RESET;
        } 
        else {
            // bytesReceived <= 0 means an error or disconnection occurred
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\nDisconnected from server." << std::endl;
            break; // Exit the loop and the thread
        }
    }
}

/**
 * @brief The main execution function for the chat client.
 * @return int Exit status (0 for success, non-zero for error).
 */
int main() {
    // 1. Create socket: AF_INET (IPv4), SOCK_STREAM (TCP), 0 (Default protocol)
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // 2. Server details: Define the server's address structure
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    // htons converts the port number to network byte order
    serverAddr.sin_port = htons(4000); 
    // Convert "127.0.0.1" (loopback address) to binary form and store in sin_addr
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // 3. Connect to server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to the server. Ensure server is running." << std::endl;
        // On connection failure, close the socket and exit
        close(clientSocket);
        return 1;
    }

    std::cout << GREEN << "Connected to server." << RESET << std::endl;

    // 4. LOGIN flow: Get username from user
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);

    // Send the login command to the server (e.g., "LOGIN Alice")
    std::string loginCmd = "LOGIN " + username;
    send(clientSocket, loginCmd.c_str(), loginCmd.size(), 0);

    // 5. Start receiver thread: This thread will handle all incoming messages
    std::thread receiver(receiveMessages, clientSocket);

    // 6. Sending loop: The main thread handles user input and message sending
    std::string message;
    std::cout << YELLOW << "You: " << RESET; // Initial prompt

    while (true) {
        // Get the entire line of input from the user
        std::getline(std::cin, message);

        // Check for the exit command
        if (message == "/quit") {
            // Send the quit command to the server so it can clean up
            send(clientSocket, message.c_str(), message.size(), 0);
            break; // Exit the sending loop
        }

        // Send the user's message to the server
        ssize_t sent = send(clientSocket, message.c_str(), message.size(), 0);
        
        if (sent <= 0) {
            // Handle send failure (e.g., socket closed)
            std::cout << "\nFailed to send message. Server may be offline." << std::endl;
            break;
        }
    }

    // 7. Cleanup and Exit
    close(clientSocket); // Close the socket file descriptor
    receiver.join();     // Wait for the receiver thread to finish its execution

    return 0;
}