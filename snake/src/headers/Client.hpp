#pragma once

#include "socket_util.hpp"

class TcpClient {
public:
    TcpClient() : clientSocket(-1) {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock\n";
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket\n";
            WSACleanup();
        }
    }

    ~TcpClient() {
        closeConnection();
    }

    bool connectToServer(const char* ipAddress, int port) {

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);  // Use the same port as the server
        serverAddr.sin_addr.s_addr = inet_addr(ipAddress);  // Use the server's IP address

        // Connect to the server
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error connecting to server\n";
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        std::cout << "Connected to server at " << ipAddress << ":" << port << "\n";
        return true;
    }

    bool send(const char* data, size_t size) {
        if (clientSocket == -1) {
            std::cerr << "Not connected to a server\n";
            return false;
        }

        ssize_t bytesSent = ::send(clientSocket, data, size, 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending data\n";
            return false;
        }

        std::cout << "Sent data to server: " << data << "\n";
        return true;
    }

    bool receive(char* buffer, size_t bufferSize) {
        if (clientSocket == -1) {
            std::cerr << "Not connected to a server\n";
            return false;
        }

        ssize_t bytesRead = ::recv(clientSocket, buffer, bufferSize - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data\n";
            closeConnection();
            return false;
        } else if (bytesRead == 0) {
            std::cout << "Server disconnected\n";
            closeConnection();
            return false;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the received data
        std::cout << "Received data from server: " << buffer << "\n";
        return true;
    }

    void closeConnection() {
        if (clientSocket != -1) {
            #ifdef _WIN32
            closesocket(clientSocket);
            #else
            close(clientSocket);
            #endif
            clientSocket = -1;
            std::cout << "Connection closed\n";
        }
    }

private:
    int createSocket() {
        #ifdef _WIN32
        return socket(AF_INET, SOCK_STREAM, 0);
        #else
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        #endif
    }

    void setupSockAddr(sockaddr_in& addr, const char* ipAddress, int port) {
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ipAddress);
    }

private:
    SOCKET clientSocket;
    WSADATA wsaData;
};

