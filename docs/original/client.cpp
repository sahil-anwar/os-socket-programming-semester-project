#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

#define PORT 8094
#define BUFFER_SIZE 1024

void receive_messages(int socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
        cerr << "Disconnected from server or error receiving message.\n";
            break;
        }
        cout << "Broadcast message received: " << buffer << "\n";
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "Failed to create socket.\n";
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed.\n";
        return 1;
    }

    cout << "Connected to server.\n";

    // Start a thread to receive messages
    thread(receive_messages, client_socket).detach();

    // Main loop for sending messages
    while (true) {
        string message;
        getline(cin, message);
        if (message == "exit") {
            break;
        }
        send(client_socket, message.c_str(), message.size(), 0);
    }

    close(client_socket);
    return 0;
}
