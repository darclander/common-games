#pragma once

#include "socket_util.hpp"
#include <SDL2/SDL.h>
#include <sstream>
#include <vector>
#include "Grid.hpp"
#include "Snake.hpp"


std::unordered_map<std::string, std::string> parseInput(const std::string& input) {
    std::unordered_map<std::string, std::string> keyValueMap;

    std::istringstream iss(input);
    std::string pair;

    while (std::getline(iss, pair, ';')) {
        std::istringstream pairStream(pair);
        std::string key, value;

        if (std::getline(pairStream, key, ':') && std::getline(pairStream, value)) {
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            keyValueMap[key] = value;
        }
    }

    return keyValueMap;
}


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
        // Ignore everything after '#'
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

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

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

class TcpClient {
    public:
        TcpClient(SDL_Renderer *renderer, Grid *grid, std::unordered_map<int, std::shared_ptr<Snake>> *players) : clientSocket(-1) {
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock\n";
            }

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Error creating socket\n";
                WSACleanup();
            }

            m_renderer = renderer;
            m_grid = grid;
            m_players = players;
            // m_players->push_back(Snake());
        }

        ~TcpClient() {
            closeConnection();
        }


        void receiveData() {
            char responseBuffer[1024];
            int resp;
            std::string input;
            while (true) {
                // Example: Receiving a response from the server
                
                resp = receive(input);
                
                std::vector<std::string> parsedInput = splitString(input, ';');
                for (auto x : parsedInput) std::cout << x << std::endl;
                if(parsedInput[0] == "PLAYER_INFO") {
                    int index = std::stoi(parsedInput[1]);
                    int xPos = std::stoi(parsedInput[2]);
                    int yPos = std::stoi(parsedInput[3]);
                    std::cout << "Creating snake at: " << xPos << ", " << yPos << std::endl;
                    std::cout << "HERE" << std::endl;
                    std::cout << m_grid << std::endl;
                    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_renderer, xPos, yPos, m_grid, 40, 40, 3, menuc::RED);
                        

                    if (m_players->count(index) == 0) {
                            (*m_players)[index] = std::move(snake);
                    } else {
                        std::cerr << "Index " << index << " is already occupied." << std::endl;
                    }
                    
                }
                

                // // Process the received data as needed
                // std::unordered_map<std::string, std::string> recvData = parseInput(input);

                // for (auto &d : recvData) {
                //     if(d.first == "NEW_PLAYER") {
                //         // addNewPlayer();
                //     }
                // }

                if (resp == 0) {
                    m_isConnected = false;
                    return;
                }
                
            }
        }

        bool connectToServer(const char* ipAddress, int port) {

            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(12345);  // Use the same port as the server
            serverAddr.sin_addr.s_addr = inet_addr(ipAddress);  // Use the server's IP address

            // Connect to the server
            if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "Error connecting to server\n";
                m_isConnected = false;
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

            // std::cout << "Sent data to server: " << data << "\n";
            return true;
        }


        bool receive(std::string& receivedData) {
            if (clientSocket == -1) {
                std::cerr << "Not connected to a server\n";
                return false;
            }

            const size_t bufferSize = 1024; // You can adjust this based on your needs
            char buffer[bufferSize];

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
            receivedData = buffer;    // Assign the buffer to the std::string
            std::cout << "Received data from server: " << receivedData << "\n";
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

        bool isConnected() {
            return m_isConnected;
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

        SOCKET clientSocket;
        WSADATA wsaData;

        std::unordered_map<int, std::shared_ptr<Snake>> *m_players;
        Grid *m_grid;
        SDL_Renderer *m_renderer;
        bool m_isConnected = true;
};
