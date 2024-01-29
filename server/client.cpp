// client.cpp

#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")

std::unordered_map<std::string, std::string> getConfiguration(const std::string& filename) {
    std::unordered_map<std::string, std::string> key_value_pairs;

    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return key_value_pairs;
    }

    // Read each line from the file
    std::string line;
    while (std::getline(input_file, line)) {
        // Use stringstream to split the line into key and value
        std::istringstream line_stream(line);
        std::string key, value;

        // Read key and value separated by ':'
        if (std::getline(line_stream, key, ':') && std::getline(line_stream, value)) {
            // Trim leading and trailing whitespaces
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            // Insert key-value pair into the map
            key_value_pairs[key] = value;
        }
    }

    input_file.close();

    return key_value_pairs;
}

void getIpAdressAndPort(std::string &ip, int &port) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto ip_it      = config.find("ip");
    auto port_it    = config.find("port");
    
    if (ip_it   != config.end()) ip = ip_it->second;
    if (port_it != config.end()) port = std::stoi(port_it->second);
}


int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return -1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket\n";
        WSACleanup();
        return -1;
    }

    // Specify the server address and port
    std::string ipAddress = "127.0.0.1";
    int port = 0;

    getIpAdressAndPort(ipAddress, port);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  // Use the same port as the server
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());  // Use the server's IP address

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server\n";
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // Loop to send and receive multiple requests
    for (int i = 0; i < 5; ++i) {
        // Send a command to the server
        const char* command = "GET_PLAYER_LIST";
        if (send(clientSocket, command, strlen(command), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending command\n";
            break;
        }

        std::cout << "Sent command to server: " << command << "\n";

        // Receive the response from the server
        char responseBuffer[1024];
        size_t responseBytesRead = recv(clientSocket, responseBuffer, sizeof(responseBuffer), 0);

        if (responseBytesRead == -1) {
            std::cerr << "Error receiving response\n";
            break;
        } else if (responseBytesRead == 0) {
            std::cout << "Server disconnected\n";
            break;
        }

        responseBuffer[responseBytesRead] = '\0'; // Null-terminate the received response
        std::cout << "Received response from server: " << responseBuffer << "\n";
    }

    // Close the socket and clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
