#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

#define SERVER_IP "127.0.0.1" // Replace with server laptop's IP address
#define PORT 8080

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IP address to binary format
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    cout << "Connected to server\n";

    // Send and receive messages
    while (true)
    {
        cout << "Client: ";
        string message;
        getline(cin, message);
        send(sock, message.c_str(), message.length(), 0);

        int read_size = read(sock, buffer, 1024);
        buffer[read_size] = '\0';
        cout << "Server: " << buffer << "\n";
    }

    close(sock);
    return 0;
}
