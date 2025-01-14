#include <iostream>
#include "Server.h"

int main() {
    int port = 8080; 
    Server server(port);
    server.start();

    return 0;
}
