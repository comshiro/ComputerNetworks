#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

class ClientHandler {
public:
    ClientHandler(int clientSocket);
    void operator()();

private:
    void processCommand(const std::string& command);
    void handleCommands();

    int clientSocket;
    std::queue<std::string> updateQueue;
    std::queue<std::string> readQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
};

#endif // CLIENTHANDLER_H