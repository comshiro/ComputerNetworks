#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class Client {
private:
    int socketFd;
    std::string serverAddress;
    int serverPort;
    struct sockaddr_in serverAddr;
    bool running;
public:
    // Constructor to initialize the server address and port
    Client(const std::string& address, int port);
    
    // Destructor to close the socket if it's open
    ~Client();
    
    // Method to establish a connection to the server
    void connectToServer();
    
    // Method to send a command to the server
    void sendCommand(const std::string& command);
    
    // Method to receive a response from the server
    std::string receiveResponse();
    
    // Method to run the client (connect, send command, receive response)
    void start();
    
    // Method to close the socket manually
    void closeConnection();
};

#endif // CLIENT_H
