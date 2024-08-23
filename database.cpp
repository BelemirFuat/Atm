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
        std::cout << "From client " << client_socket << " : " << buffer << std::endl;
        buffer[valread] = '\0';
        std::string str(buffer);
        std::vector<std::string> cmd = splitString(str, ' ');
        if (cmd[0] == "quit")
            break;
        else if (cmd[0] == "query")
        {
            std::string userNameQ = cmd[1];
            std::string PINQ = cmd[2];

            unsigned long long hash = stringToUniqueNumber(userNameQ);
            std::string fileName = std::to_string(hash);
            std::stringstream filePath;
            filePath << "database/"<<fileName<<".txt";
            std::fstream userFile(filePath.str(), std::ios::in);
            if (!userFile.is_open())
            {
                std::string reply = "401: There is no user with given user name. please try again.";
                std::cout<<reply<<" "<<filePath.str()<<std::endl;
                send(client_socket, reply.c_str(), sizeof(reply), 0);
            }
            else
            {
                std::string line;
                std::getline(userFile, line);
                std::vector<std::string> lineParsed = splitString(line, ':');
                std::string dePin = decrypt(PINQ, userNameQ);
                if (lineParsed[1] == dePin)
                {
                    std::string reply = "200: Login successfull. Welcome.";
                    logMessage("Successful login for " + userNameQ, databaseFileName);
                }
                else
                {
                    std::string reply = "401: Wrong PIN or User Name. Please try again.";
                    send(client_socket, reply.c_str(), sizeof(reply), 0);
                    logMessage("Unsuccessful login attempt for " + userNameQ, databaseFileName);
                }
            }
        }
        else if (cmd[0] == "addUser")
        {
            std::fstream userFile("database/" + cmd[1] + ".txt", std::ios::out);
            if (!userFile.is_open())
            {
                std::string reply = "There's a problem with file system. please try again later";
                std::cout << "There's a problem with file system. Attention needed";
                logMessage("There'a a problem with file system. Attention needed", databaseFileName);
                send(client_socket, reply.c_str(), sizeof(reply), 0);
            }
        }
        else if (cmd[0] == "shutdown")
        {
            exit(1);
        }
        else if (cmd[0] == "deleteUser")
        {
        }
        else if (cmd[0] == "infoUser")
        {
        }
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
