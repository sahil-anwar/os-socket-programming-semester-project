#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int yes = 1; setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    sockaddr_in addr{}; addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); addr.sin_port = htons(8084);
    if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0 || listen(server, 3) < 0) {
        perror("bind/listen"); close(server); return 1;
    }
    std::cout << "\033[35mOS LAB / DISTRIBUTED SERVER\033[0m\nWaiting on port 8084...\n";
    int client = accept(server, nullptr, nullptr);
    if (client < 0) { perror("accept"); close(server); return 1; }
    std::cout << "Client connected. Type /quit to exit.\n";
    char buf[1024];
    while (true) {
        ssize_t n = recv(client, buf, sizeof(buf), 0);
        if (n <= 0) break;
        std::cout << "Client: " << std::string(buf, static_cast<size_t>(n)) << "\nServer > " << std::flush;
        std::string reply;
        if (!std::getline(std::cin, reply) || reply == "/quit") break;
        if (send(client, reply.data(), reply.size(), MSG_NOSIGNAL) < 0) break;
    }
    shutdown(client, SHUT_RDWR); close(client); close(server);
}
