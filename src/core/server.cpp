//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file server.cpp
/// Server interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#include "server.h"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <unistd.h>

namespace core {

Server::Server(std::uint16_t port) {
    try {
        struct sockaddr_in server_addr{};
        server_addr.sin_family      = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port        = htons(port);
        this->server_socket_        = Socket(server_addr);
        this->server_socket_.listen();
        this->is_running_.store(true);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("server constructor: ") +
                                 e.what());
    }
}

Server::~Server() { stop(); }

void Server::run() noexcept {
    this->server_thread_ = std::thread([this]() { accept_loop(); });
}

void Server::stop() noexcept {
    if (!this->is_running_.exchange(false)) {
        return;
    }

    if (this->server_thread_.joinable()) {
        this->server_thread_.join();
    }

    std::vector<std::thread> threads_to_join;
    {
        std::lock_guard<std::mutex> lock(this->clients_mutex_);
        for (auto &[sock_fd, thread] : this->client_threads_) {
            ::shutdown(sock_fd, SHUT_RDWR);
            if (thread.joinable()) {
                threads_to_join.push_back(std::move(thread));
            }
        }

        this->client_threads_.clear();
        this->client_sock_fds_.clear();
    }

    for (auto &thread : threads_to_join) {
        thread.join();
    }
}

void Server::accept_loop() {
    try {
        while (this->is_running_.load()) {
            int         client_sock_fd = this->server_socket_.accept();
            std::thread client_thread(
                &Server::client_loop, this, client_sock_fd);
            {
                std::lock_guard<std::mutex> lock(this->clients_mutex_);
                this->client_sock_fds_.push_back(client_sock_fd);
                this->client_threads_.emplace(client_sock_fd,
                                              std::move(client_thread));
            }

            std::printf("[+] Client connected: fd=%d\n", client_sock_fd);
        }
    } catch (const std::exception &e) {
        if (this->is_running_.load()) {
            throw std::runtime_error(std::string("accept_loop: ") + e.what());
        }
    }
}

void Server::client_loop(int client_sock_fd) {
    try {
        Socket client_socket(client_sock_fd);
        while (this->is_running_.load()) {
            std::string message = client_socket.recv_line();
            if (message.empty()) {
                break; // Client disconnected
            }

            std::printf("[+] Received from fd=%d: %s\n",
                        client_sock_fd,
                        message.c_str());

            broadcast(message, client_sock_fd);
        }
    } catch (const std::exception &e) {
        if (this->is_running_.load()) {
            std::fprintf(stderr,
                         "[-] client_loop(fd=%d): %s\n",
                         client_sock_fd,
                         e.what());
        }
    }

    {
        std::lock_guard<std::mutex> lock(this->clients_mutex_);
        this->client_sock_fds_.erase(std::remove(this->client_sock_fds_.begin(),
                                                 this->client_sock_fds_.end(),
                                                 client_sock_fd),
                                     this->client_sock_fds_.end());

        auto it = this->client_threads_.find(client_sock_fd);
        if (it != this->client_threads_.end()) {
            this->client_threads_.erase(it);
        }
    }

    ::close(client_sock_fd);
    std::printf("[-] Client disconnected: fd=%d\n", client_sock_fd);
}

void Server::broadcast(const std::string_view message, int exclude_sock_fd) {
    std::lock_guard<std::mutex> lock(this->clients_mutex_);
    for (int client_sock_fd : this->client_sock_fds_) {
        if (client_sock_fd != exclude_sock_fd) {
            ssize_t sent = ::send(client_sock_fd,
                                  std::string(message).c_str(),
                                  message.size(),
                                  0);
            if (sent < 0) {
                throw std::runtime_error("broadcast: send failed");
            }
        }
    }
}

} // namespace core
