// database program
#include "includes.h"

std::vector<int> clients;
std::mutex mtx;

const std::string databaseFileName = "database";

void handle_client(int client_socket)
{
    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, 1024)) > 0)
    {
        buffer[valread] = '\0';
        std::cout << "From client " << client_socket << " : " << buffer << std::endl;
        std::string str(buffer);
        std::vector<std::string> cmd = splitString(str, ' ');

        if (cmd[0] == "quit")
            break;
        else if (cmd[0] == "query")
        {
            if (cmd.size() < 3)
            {
                std::string reply = "400: Bad request. Missing parameters.";
                send(client_socket, reply.c_str(), reply.size(), 0);
                continue;
            }

            std::string userNameQ = cmd[1];
            std::string PINQ = cmd[2];

            unsigned long long hash = stringToUniqueNumber(userNameQ);
            std::string fileName = std::to_string(hash);
            std::stringstream filePath;
            filePath << "database/" << fileName << ".txt";
            std::fstream userFile(filePath.str(), std::ios::in);
            if (!userFile.is_open())
            {
                std::string reply = "401: There is no user with given user name. Please try again.";
                std::cout << reply << " " << filePath.str() << std::endl;
                send(client_socket, reply.c_str(), reply.size(), 0);
                logMessage("Unsuccessful login attempt for " + userNameQ + " from IP: " + getIPAdress(client_socket), databaseFileName);
            }
            else
            {
                std::string line;
                std::getline(userFile, line);
                std::getline(userFile, line);
                std::vector<std::string> lineParsed = splitString(line, ':');
                std::string dePin = decrypt(PINQ, userNameQ); 
                if (lineParsed[1] == dePin)
                {
                    std::string reply = "200: Login successful. Welcome.";
                    send(client_socket, reply.c_str(), reply.size(), 0);
                    logMessage("Successful login for " + userNameQ + " from IP: " + getIPAdress(client_socket), databaseFileName);
                }
                else
                {
                    std::string reply = "401: Wrong PIN or User Name. Please try again.";
                    send(client_socket, reply.c_str(), reply.size(), 0);
                    logMessage("Unsuccessful login attempt for " + userNameQ + " from IP: " + getIPAdress(client_socket), databaseFileName);
                }
            }
        }
        else if (cmd[0] == "addUser")
        {
            if (cmd.size() < 3)
            {
                std::string reply = "400: Bad request. Missing parameters.";
                send(client_socket, reply.c_str(), reply.size(), 0);
                continue;
            }

            std::string userName = cmd[1];
            std::string PIN = cmd[2];

            unsigned long long hash = stringToUniqueNumber(userName);
            std::string fileName = std::to_string(hash);
            std::stringstream filePath;
            filePath << "database/" << fileName << ".txt";

            std::fstream userFile(filePath.str(), std::ios::out);
            if (!userFile.is_open())
            {
                std::string reply = "500: There's a problem with the file system. Please try again later.";
                std::cout << reply << std::endl;
                logMessage("There'a a problem with the file system. Attention needed", databaseFileName);
                send(client_socket, reply.c_str(), reply.size(), 0);
            }
            else
            {
                // Encrypt PIN before storing
                std::string encryptedPIN = decrypt(PIN, userName);
                userFile << userName << ":" << encryptedPIN << std::endl;
                userFile.close();
                std::string reply = "200: User added successfully.";
                send(client_socket, reply.c_str(), reply.size(), 0);
                logMessage("User added: " + userName, databaseFileName);
            }
        }
        else if (cmd[0] == "shutdown")
        {
            exit(1);
        }
        else if (cmd[0] == "deleteUser")
        {
            // Implement delete user functionality
        }
        else if (cmd[0] == "infoUser")
        {
            // Implement info user functionality
        }
        for(int i = 0; i<sizeof(buffer)/sizeof(char);i++ )
            buffer[i] = 0;
    }

    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    close(client_socket);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8001);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening" << std::endl;

    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        std::lock_guard<std::mutex> lock(mtx);
        clients.push_back(new_socket);
        std::cout << "New client: ID: " << new_socket << " \n";
        std::cout << "New cliend IP : " << getIPAdress(new_socket) << std::endl;
        logMessage("connected to database with IP: " + getIPAdress(new_socket), databaseFileName);
        std::thread(handle_client, new_socket).detach();
    }

    close(server_fd);
    return 0;
}
