#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace ui {
constexpr auto purple = "\033[35m";
constexpr auto green = "\033[32m";
constexpr auto reset = "\033[0m";
void banner(const std::string& name) {
    std::cout << purple << "\n  +----------------------------------------+\n"
              << "  |  OS LAB  /  " << name << "\n"
              << "  +----------------------------------------+\n" << reset;
}
}

constexpr int PORT = 8094;
constexpr size_t MAX_MESSAGE = 1024;
std::vector<int> clients;
std::mutex clients_mutex;
std::mutex output_mutex;
std::atomic<bool> running{true};

void log(const std::string& message) {
    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << message << std::endl;
}

bool send_all(int socket_fd, const std::string& text) {
    size_t sent = 0;
    while (sent < text.size()) {
        ssize_t n = send(socket_fd, text.data() + sent, text.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

void broadcast(const std::string& text) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int fd : clients) {
        if (!send_all(fd, text + "\n")) log("[warning] Could not deliver to a client.");
    }
}

void handle_client(int fd) {
    char buffer[MAX_MESSAGE];
    std::string pending;
    while (running) {
        ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        pending.append(buffer, static_cast<size_t>(n));
        size_t pos;
        while ((pos = pending.find('\n')) != std::string::npos) {
            std::string message = pending.substr(0, pos);
            pending.erase(0, pos + 1);
            if (!message.empty()) {
                log("[client " + std::to_string(fd) + "] " + message);
                broadcast("Client " + std::to_string(fd) + ": " + message);
            }
        }
        if (pending.size() > 8192) { log("[warning] Message too long."); break; }
    }
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), fd), clients.end());
    }
    close(fd);
    log("[-] Client disconnected");
}

int main() {
    std::signal(SIGPIPE, SIG_IGN);
    ui::banner("BROADCAST SERVER");
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int reuse = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0 || listen(server, 10) < 0) {
        perror("bind/listen"); close(server); return 1;
    }
    log(std::string(ui::green) + "[ready] Listening on port " + std::to_string(PORT) + ui::reset);
    log("Type a message to broadcast, or /quit to stop.\n");
    std::thread accept_thread([&] {
        while (running) {
            sockaddr_in peer{}; socklen_t length = sizeof(peer);
            int fd = accept(server, reinterpret_cast<sockaddr*>(&peer), &length);
            if (fd < 0) { if (!running) break; continue; }
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(fd);
            }
            log("[+] Client connected: " + std::string(inet_ntoa(peer.sin_addr)));
            std::thread(handle_client, fd).detach();
        }
    });
    std::string message;
    while (std::getline(std::cin, message)) {
        if (message == "/quit" || message == "exit") break;
        if (!message.empty()) broadcast("Server: " + message);
    }
    running = false;
    shutdown(server, SHUT_RDWR);
    close(server);
    if (accept_thread.joinable()) accept_thread.join();
    // Lab demo: detached client threads finish when process exits.
    return 0;
}
