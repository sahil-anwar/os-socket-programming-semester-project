#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    const std::string host = argc > 1 ? argv[1] : "127.0.0.1";
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8094);
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1 ||
        connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        perror("connect"); close(fd); return 1;
    }
    std::cout << "\033[35m\n  OS LAB / BROADCAST CLIENT\033[0m\n"
              << "[connected] " << host << ":8094\nType messages, /quit to leave.\n\n";
    std::atomic<bool> connected{true};
    std::thread receiver([&] {
        char buffer[1024]; std::string pending;
        while (connected) {
            ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            pending.append(buffer, static_cast<size_t>(n));
            size_t pos;
            while ((pos = pending.find('\n')) != std::string::npos) {
                std::cout << "\n\033[35m< \033[0m" << pending.substr(0, pos) << "\n> " << std::flush;
                pending.erase(0, pos + 1);
            }
        }
        connected = false;
        std::cout << "\n[disconnected]\n";
    });
    std::string line;
    while (connected && (std::cout << "> " << std::flush, std::getline(std::cin, line))) {
        if (line == "/quit" || line == "exit") break;
        if (line.empty()) continue;
        line += "\n";
        if (send(fd, line.data(), line.size(), MSG_NOSIGNAL) < 0) break;
    }
    connected = false;
    shutdown(fd, SHUT_RDWR);
    if (receiver.joinable()) receiver.join();
    close(fd);
}
