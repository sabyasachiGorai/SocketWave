
# ⚡ SOCKETWAVE – TCP Chat Application
A lightweight, multi-user chat system built using:

- **Node.js (TCP Chat Server + Express API)**
- **C++ Linux Client**
- **C++ Windows Client**

This project demonstrates TCP networking, sockets, threading, and basic protocol design — ideal for an Object-Oriented Programming assignment.

---

## 📁 Project Structure

```

SOCKETWAVE/
│
├── backend/
│   ├── package.json
│   ├── package-lock.json
│   └── server.js                 # Node.js TCP + HTTP server
│
├── chat_client_linux.cpp         # Linux C++ chat client (multithreaded)
├── chat_client_win.cpp           # Windows C++ chat client (winsock version)
│
└── README.md

````

---

# 🚀 1. Features

### 🟢 Server (Node.js)
- Accepts multiple TCP clients simultaneously  
- LOGIN system (`LOGIN <username>`)  
- Broadcasts messages to all users  
- Notifies when users join or leave  
- Simple HTTP API using Express  
  - `/users` → list connected users  
  - `/` → server status  

### 🔵 Linux C++ Client
- Automatic login prompt  
- Multi-threaded message receiving  
- Colored chat output (ANSI-based)  
- Timestamps on every message  
- Graceful exit using `/quit`  

### 🟣 Windows C++ Client
- Same functionality as Linux version  
- Uses WinSock2  
- Works on Windows Terminal / PowerShell  

---

# ⚙️ 2. Requirements

### Server
- Node.js v16+
- npm  

### Linux Client
- g++ compiler  
- POSIX socket support (Ubuntu / WSL / Codespaces)  

### Windows Client
- MinGW g++ or Visual Studio Build Tools  
- WinSock2 installed by default  

---

# 🖥️ 3. Running the Node.js Server

### Step 1 — Move into backend directory
```bash
cd backend
````

### Step 2 — Install dependencies

```bash
npm install
```

### Step 3 — Start the server

```bash
node server.js
```

You should see:

```
TCP chat server listening on port 4000
Express HTTP server listening on port 3000
```

---

# 🔗 4. Testing Server API (Optional)

### List Connected Users

```
http://localhost:3000/users
```

### Server Status

```
http://localhost:3000/
```

---

# 🧪 5. Running the Linux C++ Client

### Step 1 — Compile

```bash
g++ chat_client_linux.cpp -o chatclient -pthread
```

### Step 2 — Run

```bash
./chatclient
```

Then enter your username when prompted.

---

# 🪟 6. Running the Windows C++ Client

### Step 1 — Compile using MinGW

```bash
g++ chat_client_win.cpp -o chatclient.exe -lws2_32
```

### Step 2 — Run

```
chatclient.exe
```

---

# 💬 7. Chat Usage

After login, type messages normally.

### Exit the chat

```
/quit
```

### Example interaction

```
[17:40] SERVER: Alice has joined the chat  
[17:41] Alice: Hi everyone!  
You: Hello Alice!  
```

---

# 🔌 8. How the Protocol Works

| Action       | Message Format     |
| ------------ | ------------------ |
| Login        | `LOGIN <username>` |
| Chat message | `<text>`           |
| Quit         | `/quit`            |

Server broadcasts messages to all connected users except the sender.

---

# 🛠️ 9. Troubleshooting

### ❌ "Connection refused"

* Make sure server is running on **port 4000**
* Make sure you are using this IP:

```
127.0.0.1
```

### ❌ Linux client stops receiving messages

* Ensure you compiled with:

```
-pthread
```

### ❌ Windows compile error: ws2_32 not found

Add the library flag:

```
-lws2_32
```

---

