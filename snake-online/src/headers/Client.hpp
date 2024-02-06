#pragma once

#include "socket_util.hpp"
#include <SDL2/SDL.h>
#include <sstream>
#include <vector>

#include "Grid.hpp"
#include "Gui.hpp"
#include "Snake.hpp"
#include "Score.hpp"

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
        TcpClient(SDL_Renderer *renderer, GUI *gui, Grid *grid, std::unordered_map<int, std::shared_ptr<Snake>> *players, std::vector<std::shared_ptr<Score>> *scores) : clientSocket(-1) {
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock\n";
            }

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Error creating socket\n";
                WSACleanup();
            }

            m_renderer  = renderer;
            m_grid      = grid;
            m_players   = players;
            m_scores    = scores;
            m_gui       = gui;
            // m_players->push_back(Snake());
        }

        ~TcpClient() {
            closeConnection();
        }

        void setPid(int pid) {
            m_pid = pid;
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
                if((parsedInput[0] == "PLAYER_INFO") || (parsedInput[0] == "NEW_PLAYER")) {
                    int pid = std::stoi(parsedInput[1]);
                    int xPos = std::stoi(parsedInput[2]);
                    int yPos = std::stoi(parsedInput[3]);
                    std::cout << "Creating snake at: " << xPos << ", " << yPos << std::endl;
                    // std::cout << "HERE" << std::endl;
                    // std::cout << m_grid << std::endl;
                    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_renderer, xPos, yPos, m_grid, 40, 40, 3, menuc::RED);
                        

                    if (m_players->count(pid) == 0) {
                            (*m_players)[pid] = std::move(snake);
                    } else {
                        std::cerr << "Index " << pid << " is already occupied." << std::endl;
                    }
                } else if(parsedInput[0] == "PLAYER_NEW_POS") {
                    std::cout << "Received new pos" << std::endl;
                    int pid = std::stoi(parsedInput[1]);
                    int xPos = std::stoi(parsedInput[2]);
                    int yPos = std::stoi(parsedInput[3]);

                    auto iterator = m_players->find(pid);
                    if (iterator != m_players->end()) {
                        auto &s = iterator->second;
                        if (s->getPosX() != xPos || s->getPosY() != yPos) {
                            iterator->second->updatePos(xPos, yPos);
                            iterator->second->getPositions();
                        }
                    } else {

                    }
                } else if (parsedInput[0] == "ADD_SCORE") {
                    std::shared_ptr<Score> score = std::make_shared<Score>(m_renderer, m_gui, m_grid->getGridPointWidth(), m_grid->getGridPointWidth());
                    int xPos = std::stoi(parsedInput[3]);
                    int yPos = std::stoi(parsedInput[4]);
                    // std::cout << "xPos: " <<  xPos << std::endl;
                    xPos = (xPos) * (m_grid->getGridPointWidth());
                    yPos = (yPos) * (m_grid->getGridPointHeight());

                    Gridpoint *gp = m_grid->getPoint(xPos, yPos);
                    gp->setScore();
                    score->move(gp->getGridPointX(), gp->getGridPointY());
                    m_scores->push_back(score);
                } else if (parsedInput[0] == "SCORE_COLLECTED") {
                    int pid = std::stoi(parsedInput[1]);
                    // auto it = std::find(m_scores->begin(), m_scores->end(), score);
                    // if (it != m_scores->end()) {
                    //     m_scores->erase(it);
                    // }
                    auto iterator = m_players->find(pid);
                    if (iterator != m_players->end()) {
                        iterator->second->grow();
                    }
                    
                }
                

                // // Process the received data as needed
                // std::unordered_map<std::string, std::string> recvData = parseInput(input);

                // for (auto &d : recvData) {
                //     if(d.first == "NEW_PLAYER") {
                //         // addNewPlayer();
                //     }
                // }
                input = "";
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
            // std::cout << "Received data from server: " << receivedData << "\n";
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
        std::vector<std::shared_ptr<Score>> *m_scores;
        
        Grid *m_grid;
        GUI *m_gui;
        int m_pid;
        SDL_Renderer *m_renderer;
        bool m_isConnected = true;
};
