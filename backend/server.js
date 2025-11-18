// server.js
const express = require('express');
const net = require('net');

const app = express();
const HTTP_PORT = 3000;
const TCP_PORT = 4000;

// Keep track of connected clients
// Map socket -> { username, socket }
const clients = new Map();

// Helper broadcast function
function broadcast(senderSocket, message) {
  for (const [sock, info] of clients.entries()) {
    if (sock !== senderSocket) {
      sock.write(message + '\n');
    }
  }
}

// Start TCP chat server
const tcpServer = net.createServer((socket) => {
  socket.setEncoding('utf8');
  let loggedIn = false;
  let username = null;

  socket.write('WELCOME: send "LOGIN <username>" to join\n');

  socket.on('data', (data) => {
    // data may contain multiple lines; handle line-by-line
    const lines = data.split(/\r?\n/).filter(Boolean);
    for (const line of lines) {
      if (!loggedIn) {
        const parts = line.trim().split(' ');
        if (parts[0] === 'LOGIN' && parts[1]) {
          username = parts.slice(1).join(' ');
          loggedIn = true;
          clients.set(socket, { username, socket });
          console.log(`User logged in: ${username}`);
          socket.write(`LOGIN_OK Welcome, ${username}\n`);
          broadcast(socket, `SERVER: ${username} has joined the chat`);
        } else {
          socket.write('ERROR You must login first with: LOGIN <username>\n');
        }
      } else {
        const text = line.trim();
        if (text.toLowerCase() === '/quit') {
          socket.end('BYE\n');
        } else {
          const out = `${username}: ${text}`;
          console.log('MSG ->', out);
          broadcast(socket, out);
        }
      }
    }
  });

  socket.on('close', () => {
    if (loggedIn) {
      clients.delete(socket);
      console.log(`User disconnected: ${username}`);
      broadcast(socket, `SERVER: ${username} has left the chat`);
    }
  });

  socket.on('error', (err) => {
    console.log('Socket error', err.message);
  });
});

tcpServer.listen(TCP_PORT, () => {
  console.log(`TCP chat server listening on port ${TCP_PORT}`);
});

// Simple express HTTP endpoint to list connected users
app.get('/users', (req, res) => {
  const users = [];
  for (const [, info] of clients.entries()) {
    users.push(info.username);
  }
  res.json({ users });
});

app.get('/', (req, res) => {
  res.send('Node.js + TCP chat server is running. Use a TCP client to connect on port ' + TCP_PORT);
});

app.listen(HTTP_PORT, () => {
  console.log(`Express HTTP server listening on port ${HTTP_PORT}`);
});
