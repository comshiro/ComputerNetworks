
#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

Client::Client(const std::string &address, int port)
    : serverAddress(address), serverPort(port), socketFd(-1)
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
}

Client::~Client()
{
    if (socketFd != -1)
    {
        close(socketFd);
    }
}

void Client::connectToServer()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Connection to server failed");
        close(socketFd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected to server at " << serverAddress << ":" << serverPort << std::endl;
}

void Client::sendCommand(const std::string &command)
{
    if (send(socketFd, command.c_str(), command.length(), 0) == -1)
    {
        perror("Send failed");
        close(socketFd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Command sent: " << command << std::endl;
}

std::string Client::receiveResponse()
{
    char buffer[1024];
    int bytesReceived = recv(socketFd, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1)
    {
        perror("Receive failed");
        close(socketFd);
        exit(EXIT_FAILURE);
    }
    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}

void Client::start()
{
    connectToServer();
    bool running = true;

    while (running)
    {
        std::string command;
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        if (command == "exit")
        {
            running = false;
            closeConnection();
        }
        else
        {
            sendCommand(command);

            std::string response = receiveResponse();
            std::cout << "Response from server: " << response << std::endl;
        }
    }
}

void Client::closeConnection()
{
    close(socketFd);
    socketFd = -1;
}
