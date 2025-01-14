#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include "ClientHandler.h"

class Server {
private:
    int serverSocket;
    struct sockaddr_in serverAddress;  
    std::atomic<bool> running;

    void acceptConnections();

public:
    Server(int port);
    ~Server();
    void handleClient(int clientSocket); 
    void start();
    void stop();
};

#endif
