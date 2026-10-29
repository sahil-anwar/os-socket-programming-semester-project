#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

#define PORT 8094
#define BUFFER_SIZE 1024

vector<int> client_sockets; // List of connected client sockets

// Broadcast a message to all connected clients
void broadcast_message(const string &message)
{
    for (int sock : client_sockets)
    {
        int result = send(sock, message.c_str(), message.size(), 0);
        if (result == -1)
        {
            cerr << "Failed to send message to client socket: " << sock << "\n";
        }
        else
        {
            cout << "Message sent to client socket: " << sock << "\n";
        }
    }
}

// Handle messages from a specific client
void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0)
        {
            cerr << "Client disconnected.\n";
            close(client_socket);
            client_sockets.erase(
                remove(client_sockets.begin(), client_sockets.end(), client_socket),
                client_sockets.end());
            break;
        }

        cout << "Received from client: " << buffer << "\n";
        broadcast_message(buffer);
    }
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "Failed to create socket.\n";
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Bind failed.\n";
        return 1;
    }

    if (listen(server_socket, 5) < 0)
    {
        cerr << "Listen failed.\n";
        return 1;
    }

    cout << "Server is listening on port " << PORT << "...\n";

    // Thread for accepting new clients
    thread([&]()
           {
        while (true) {
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (client_socket < 0) {
                cerr << "Accept failed.\n";
                continue;
            }
            cout << "New client connected.\n";
            client_sockets.push_back(client_socket);

            // Handle each client in a separate thread
            thread(handle_client, client_socket).detach();
        } })
        .detach();

    // Main thread for sending messages from the server
    string server_message;
    while (true)
    {
        getline(cin, server_message);
        if (server_message == "exit")
        {
            break;
        }
        broadcast_message("Server: " + server_message);
    }

    close(server_socket);
    return 0;
}
