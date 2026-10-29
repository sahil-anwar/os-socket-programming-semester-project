#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

#define PORT 8084

int main()
{
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    cout << "Waiting for connections...\n";

    // Accept a connection from the client
    if ((client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    cout << "Client connected\n";

    // Read and send messages
    while (true)
    {
        int read_size = read(client_sock, buffer, 1024);
        if (read_size == 0)
        {
            cout << "Client disconnected\n";
            break;
        }
        buffer[read_size] = '\0';
        cout << "Client: " << buffer << "\n";

        cout << "Server: ";
        string response;
        getline(cin, response);
        send(client_sock, response.c_str(), response.length(), 0);
    }

    close(client_sock);
    close(server_fd);
    return 0;
}
