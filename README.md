# **SocketWave – Real-time TCP Chat System**

A lightweight client-server chat application built using **C++ (client)** and **Node.js (server)** using **TCP sockets**.
This project demonstrates networking concepts, socket programming, OOP thinking, and real-time message broadcasting.

---

## 🚀 **Project Overview**

**SocketWave** is a real-time chat system where multiple users can connect to a Node.js TCP server and communicate using a C++ client.
It follows a classic **client–server architecture**, showing how sockets, threads, message broadcasting, and authentication work together.

The project includes:

* ✅ A **C++ client** (Linux compatible)
* ✅ A **Node.js TCP server**
* ✅ User login system
* ✅ Real-time message broadcasting to all connected clients
* ✅ `/quit` support
* ✅ Express.js API to list currently connected users

This system can run inside **GitHub Codespaces**, Linux, or any UNIX environment.

---

## 🧩 **Tech Stack**

### **Client (Frontend)**

* **C++**
* POSIX Sockets (`arpa/inet.h`, `sys/socket.h`)
* Multithreading (`std::thread`)
* Linux environment support

### **Server (Backend)**

* **Node.js**
* `net` module for raw TCP communication
* `express` for HTTP API
* Map-based session tracking

---

## 📌 **Features**

### 🔹 **User Login**

Clients must log in using:

```
LOGIN <username>
```

Before logging in, server rejects messages.

---

### 🔹 **Real-Time Chat**

After login, users can send messages that are broadcasted to all other connected clients:

```
username: message
```

---

### 🔹 **Graceful Exit**

Client can leave using:

```
/quit
```

This closes the connection and notifies others.

---

### 🔹 **HTTP API Endpoint**

List all active users:

```
GET /users
```

Response:

```json
{
  "users": ["sabyasachi", "testUser"]
}
```

---

## 📁 **Project Structure**

```
/project
│── chat_client.cpp     # C++ Linux client
│── server.js           # Node.js TCP server + Express API
│── README.md           # Project documentation
```

---

## ⚙️ **How to Run**

### 1️⃣ Start the TCP + HTTP Server (Node.js)

```bash
node server.js
```

Server runs on:

* TCP → **4000**
* HTTP → **3000**

---

### 2️⃣ Compile the C++ Client (Linux)

```bash
g++ chat_client.cpp -o client -pthread
```

### 3️⃣ Run the Client

```bash
./client
```

You’ll see:

```
Connected to the server.
WELCOME: send "LOGIN <username>" to join
```

Then log in:

```
LOGIN sabyasachi
```

Start chatting 🎉

---

## 🔍 **How OOP Concepts Apply**

Even though sockets are procedural, your architecture demonstrates:

### **1. Encapsulation**

* Receiving messages handled by a dedicated function `receiveMessages()`
* Sending handled in main loop

### **2. Abstraction**

* User interacts only through commands (`LOGIN`, `/quit`)
* Networking logic hidden behind sockets

### **3. Responsibility Separation**

Client → handles input & display
Server → handles login, broadcasting, user tracking

### **4. Modular Design**

* Independent client and server components
* Thread handling separated from message logic

This makes it a valid and strong OOPS-related project.

---

## 🧪 **Sample Output**

```
Connected to the server.
LOGIN sabyasachi
LOGIN_OK Welcome, sabyasachi
SERVER: sammy has joined the chat
sammy: Hey!
sabyasachi: Hello!
```

---

## 🎯 **Future Improvements**

* Add private messaging
* Add user authentication with passwords
* Add a GUI client
* Add chat history storage
* Add typing indicators

---

## 🏁 **Conclusion**

**SocketWave** is a simple yet powerful demonstration of networking, sockets, threads, OOP principles, and full-stack thinking.
Perfect for academic submissions and future expansion 🚀
