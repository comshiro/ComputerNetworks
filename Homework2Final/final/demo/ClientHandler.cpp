#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "ClientHandler.h"
#include "XMLManager.h"

ClientHandler::ClientHandler(int clientSocket) : clientSocket(clientSocket) {}

void ClientHandler::processCommand(const std::string& command) {
    XMLManager xmlManager("trains.xml");

    if (command.rfind("GET_ARRIVALS", 0) == 0) {
        size_t firstSpace = command.find(' ', 12);
        std::string trainId = command.substr(12, firstSpace - 12); // Extract train ID from command
        std::string stationName = command.substr(firstSpace + 1); // Extract station name from command
        std::string response = xmlManager.getArrivals(trainId, stationName);
        write(clientSocket, response.c_str(), response.length() + 1);
    }
    else if (command.rfind("GET_DEPARTURES", 0) == 0) {
        size_t firstSpace = command.find(' ', 14);
        std::string trainId = command.substr(14, firstSpace - 14); // Extract train ID from command
        std::string stationName = command.substr(firstSpace + 1); // Extract station name from command
        std::string response = xmlManager.getDepartures(trainId, stationName);
        write(clientSocket, response.c_str(), response.length() + 1);
    }
    else if (command.rfind("GET_ROUTE", 0) == 0) {
        std::string trainId = command.substr(10); // Extract train ID from command
        std::string response = xmlManager.getRoute(trainId);
        write(clientSocket, response.c_str(), response.length() + 1);
    }
    else if (command.rfind("ANNOUNCE_DELAY", 0) == 0) {
        size_t firstSpace = command.find(' ', 14);
        size_t secondSpace = command.find(' ', firstSpace + 1);
        std::string trainId = command.substr(14, firstSpace - 14); // Extract train ID from command
        std::string stationName = command.substr(firstSpace + 1, secondSpace - firstSpace - 1); // Extract station name from command
        int delayMinutes = std::stoi(command.substr(secondSpace + 1)); // Extract delay minutes from command
        xmlManager.announceDelay(trainId, stationName, delayMinutes);
        std::string response = "Delay announced for train " + trainId + " at " + stationName;
        write(clientSocket, response.c_str(), response.length() + 1);
    }
    else {
        std::string response = "Unknown command.";
        write(clientSocket, response.c_str(), response.length() + 1);
    }
}

void ClientHandler::handleCommands() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondition.wait(lock, [this] { return !updateQueue.empty() || !readQueue.empty(); });

        while (!updateQueue.empty()) {
            std::string command = updateQueue.front();
            updateQueue.pop();
            lock.unlock();
            processCommand(command);
            lock.lock();
        }

        while (!readQueue.empty()) {
            std::string command = readQueue.front();
            readQueue.pop();
            lock.unlock();
            processCommand(command);
            lock.lock();
        }
    }
}

void ClientHandler::operator()() {
    std::thread commandHandler(&ClientHandler::handleCommands, this);
    commandHandler.detach();

    char buffer[1024];
    int bytesRead;

    // Read the command from the client
    while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "[ClientHandler] Received command: " << buffer << "\n";

        std::unique_lock<std::mutex> lock(queueMutex);
        if (strncmp(buffer, "ANNOUNCE_DELAY", 14) == 0) {
            updateQueue.push(buffer);
        } else {
            readQueue.push(buffer);
        }
        queueCondition.notify_one();
    }

    // Close the client connection after finishing
    close(clientSocket);
}