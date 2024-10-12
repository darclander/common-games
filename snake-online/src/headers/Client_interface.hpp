#ifndef CLIENT_INTERFACE_HPP
#define CLIENT_INTERFACE_HPP

#pragma once

#include "comm_util.hpp"
#include <sstream>
#include <vector>

class TcpCommunication {
    public:
        TcpCommunication(const char *ip, int port) : clientSocket(-1) {
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock\n";
            }

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Error creating socket\n";
                WSACleanup();
            }

            m_ip = ip;
            m_port = port;

            connectToServer();

        }

        ~TcpCommunication() {
            closeConnection();
        }

        void receiveData() {
            char responseBuffer[1024];
            int resp = 1;
            std::string input;
            while (true) {
                // Example: Receiving a response from the server
                

                // resp = receive(input);
                input = receiveChar(1024);
                if(input == "false") return;
                std::cout << "Received new message!" << std::endl;

                std::vector<std::string> parsedInput = splitString(input, ';');
                for (auto x : parsedInput) std::cout << x << std::endl;

                input = "";
                if (resp == 0) {
                    m_isConnected = false;
                    return;
                }
                
            }
        }

        bool send(const char* data, size_t size) {
            if (clientSocket == -1) {
                std::cerr << "Not connected to a server\n";
                m_isConnected = false;
                return false;
            }

            ssize_t bytesSent = ::send(clientSocket, data, size, 0);
            if (bytesSent == -1) {
                std::cerr << "Error sending data\n";
                return false;
            }

            // std::cout << "Sent data to server: " << data << "\n";
            return true;
        }

        std::string receiveChar(size_t bufferSize) {
            
            if (clientSocket == -1) {
                std::cerr << "Not connected to a server\n";
                m_isConnected = false;
                return "false";
            }

            char buffer[bufferSize];
            ssize_t bytesRead = ::recv(clientSocket, buffer, bufferSize - 1, 0);
            if (bytesRead == -1) {
                std::cerr << "Error receiving data\n";
                closeConnection();
                return "false";
            } else if (bytesRead == 0) {
                std::cout << "Server disconnected\n";
                closeConnection();
                return "false";
            }

            buffer[bytesRead] = '\0'; // Null-terminate the received data
            // std::cout << "Received data from server: " << buffer << "\n";
            return std::string(buffer);
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

        bool isConnected() {
            return m_isConnected;
        }
        

    private:

        const char *m_ip;
        int m_port;

        SOCKET clientSocket;
        WSADATA wsaData;

        bool m_isConnected = true;

        bool connectToServer() {

            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(m_port);  // Use the same port as the server
            serverAddr.sin_addr.s_addr = inet_addr(m_ip);  // Use the server's IP address

            // Connect to the server
            if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "Error connecting to server\n";
                m_isConnected = false;
                closesocket(clientSocket);
                WSACleanup();
                return false;
            }

            std::cout << "Connected to server at " << m_ip << ":" << m_port << "\n";
            return true;
        }

};

#endif