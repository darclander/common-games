#pragma once

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <cstring>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#endif

#ifdef _WIN32
using ssize_t = int;
#endif

// Initialize Winsock
bool initWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

// Cleanup Winsock
void cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Create a socket
int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

// Close a socket
void closeSocket(int socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

// Set up a sockaddr_in structure
void setupSockAddr(sockaddr_in& addr, const char* ipAddress, int port) {
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAddress);
}

