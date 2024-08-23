// server program
#include "includes.h"

std::vector<int> clients;
std::mutex mtx;

void handle_client(int client_socket)
{
    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, 1024)) > 0)
    {
        std::cout << "From client " << client_socket << " : " << buffer << std::endl;
        buffer[valread] = '\0';
        if (strcmp(buffer, "quit") == 0)
            break;
        else if(strcmp(buffer, "login") == 0)
        {
            char userName[33] = {0};
            char PIN[5] = {0};

            read(client_socket, userName, 33);
            read(client_socket, PIN, 5);

            userName[32] = '\0';
            PIN[4] = '\0';
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
    address.sin_port = htons(8002);

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
        std::thread(handle_client, new_socket).detach();
    }

    close(server_fd);
    return 0;
}
