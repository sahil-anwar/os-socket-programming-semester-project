#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
int main(int argc, char** argv) {
    const std::string host = argc > 1 ? argv[1] : "127.0.0.1";
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }
    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(8084);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1 ||
        connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("connect"); close(fd); return 1;
    }
    std::cout << "\033[35mOS LAB / DISTRIBUTED CLIENT\033[0m\nConnected to " << host << ":8084\n";
    std::string message; char buffer[1024];
    while (std::cout << "Client > " << std::flush, std::getline(std::cin, message)) {
        if (message == "/quit") break;
        if (send(fd, message.data(), message.size(), MSG_NOSIGNAL) < 0) break;
        ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        std::cout << "Server: " << std::string(buffer, static_cast<size_t>(n)) << "\n";
    }
    shutdown(fd, SHUT_RDWR); close(fd);
}
