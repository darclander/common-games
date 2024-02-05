// server.cpp

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>  // for std::remove

int broadcast(std::string str, std::vector<int> &clients) {
        for(auto c : clients) {
            const char* response = str.c_str();
            send(c, response, strlen(response), 0);
        }
        return 0;
}

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Bind the socket to a specific address and port
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);  // You can choose any available port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket\n";
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port 12345\n";

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(serverSocket, &readSet);

    int maxFd = serverSocket;

    // Vector to store client sockets
    std::vector<int> clientSockets;

    while (true) {
        fd_set tmpSet = readSet;

        // Wait for activity on any socket
        if (select(maxFd + 1, &tmpSet, nullptr, nullptr, nullptr) == -1) {
            std::cerr << "Error in select\n";
            break;
        }

        // Check for incoming connection
        if (FD_ISSET(serverSocket, &tmpSet)) {
            sockaddr_in clientAddr{};
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

            if (clientSocket == -1) {
                std::cerr << "Error accepting connection\n";
            } else {
                std::cout << "New client connected\n";

                FD_SET(clientSocket, &readSet);
                if (clientSocket > maxFd) {
                    maxFd = clientSocket;
                }

                // Add the new client socket to the vector
                clientSockets.push_back(clientSocket);
            }
        }

        // Check for data on existing connections
        for (auto it = clientSockets.begin(); it != clientSockets.end(); ) {
            int fd = *it;

            if (FD_ISSET(fd, &tmpSet)) {
                char buffer[1024];
                ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);

                if (bytesRead == -1) {
                    std::cerr << "Error receiving data from client\n";
                    close(fd);
                    FD_CLR(fd, &readSet);

                    // Remove the closed socket from the vector
                    it = clientSockets.erase(it);
                    broadcast("CLIENT DISCONNECTED", clientSockets);
                } else if (bytesRead == 0) {
                    std::cout << "Client disconnected\n";
                    close(fd);
                    FD_CLR(fd, &readSet);

                    // Remove the disconnected socket from the vector
                    it = clientSockets.erase(it);
                    broadcast("CLIENT DISCONNECTED", clientSockets);
                } else {
                    buffer[bytesRead] = '\0'; // Null-terminate the received data
                    std::cout << "Received from client: " << buffer << "\n";

                    // Add your asynchronous read logic here

                    // Example: Sending a response back to the client
                    const char* response = "Data received successfully!";
                    send(fd, response, strlen(response), 0);

                    ++it;  // Move to the next element in the vector
                }
            } else {
                ++it;  // Move to the next element in the vector
            }
        }
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
