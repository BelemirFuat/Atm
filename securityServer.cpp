// command encrypt(data) returns encrypted data
// command decrypt(data) returns decrypted data

#include "includes.h"

std::vector<int> clients;
std::mutex mtx;

std::string encrypt(std::string data)
{
    // just uses a bullshit encrypt algorithm
    std::string val = data;
    for (int i = 0; i < data.size(); i++)
    {
        val[i] = data[i] + 15;
    }
    return val;
}
std::string decrypt(std::string data)
{
    std::string val = data;
    for (int i = 0; i < data.size(); i++)
    {
        val[i] = data[i] - 15;
    }
    return val;
}       

void handle_client(int client_socket)
{
    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, 1024)) > 0)
    {
        std::cout << "From client " << client_socket << " : " << buffer << std::endl;
        buffer[valread] = '\0';
        std::string str(buffer);
        std::vector<std::string> cmd = splitString(str,' ');

        if (cmd[0] == "quit")
            break;

        else if (cmd[0] == "encrypt")
        {   
            std::string encrypted = encrypt(cmd[1]);
            send(client_socket, encrypted.c_str(), encrypted.size(), 0);
        }
        else if (cmd[0] == "decrypt")
        {
            std::string decrypted = decrypt(cmd[1]);
            send(client_socket, decrypted.c_str(), decrypted.size(), 0);
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
    address.sin_port = htons(8000);

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
