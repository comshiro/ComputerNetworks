#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <netinet/in.h> 
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include "ClientHandler.h"
#include "Server.h"

Server::Server(int port) {
    // Initialize server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Set server to listen for connections
    if (listen(serverSocket, 5) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    running.store(true);
}

Server::~Server() {
    close(serverSocket);
}

void Server::start() {
    std::cout << "[Server] Waiting for clients to connect...\n";
    acceptConnections();
}

void Server::stop() {
    running.store(false);
}

void Server::handleClient(int clientSocket) {
    ClientHandler clientHandler(clientSocket);
    clientHandler();
}

void Server::acceptConnections() {
    while (running.load()) {
        struct sockaddr_in clientAddress;
        socklen_t clientLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
        if (clientSocket < 0) {
            perror("[Server] Error accepting connection");
            continue;
        }

        std::cout << "[Server] Client connected!\n";

        // Create a new thread to handle the client
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
}
