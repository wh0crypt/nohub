//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file socket.cpp
/// Socket interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#include "socket.h"

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <unistd.h>

namespace core {

Socket::Socket(int sock_fd) noexcept : sock_fd_(sock_fd) {}

Socket::Socket(const std::string_view ip, std::uint16_t port) {
    try {
        this->addr_    = make_addr(ip, port);
        this->sock_fd_ = make_socket(this->addr_);
    } catch (std::invalid_argument &e) {
        throw std::invalid_argument(std::string("socket constructor: ") +
                                    e.what());
    }
}

Socket::Socket(const struct sockaddr_in &addr) {
    try {
        this->addr_    = addr;
        this->sock_fd_ = make_socket(addr);
    } catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("socket constructor: ") +
                                 e.what());
    }
}

Socket::Socket(Socket &&other) noexcept {
    this->sock_fd_ = other.sock_fd_;
    other.sock_fd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
    if (this != &other) {
        if (this->sock_fd_ >= 0) {
            ::close(this->sock_fd_);
        }

        this->sock_fd_ = other.sock_fd_;
        other.sock_fd_ = -1;
    }

    return *this;
}

Socket Socket::create_tcp_socket() {
    int sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        throw std::runtime_error(std::string("socket: ") +
                                 std::strerror(errno));
    }

    return Socket(sock_fd);
}

Socket::~Socket() {
    if (this->sock_fd_ >= 0) {
        ::close(this->sock_fd_);
    }
}

int Socket::sock_fd() const noexcept { return this->sock_fd_; }

std::string Socket::addr_str() const {
    std::string str_ip   = inet_ntoa(this->addr_.sin_addr);
    std::string str_port = std::to_string(ntohs(this->addr_.sin_port));
    return str_ip + ":" + str_port;
}

void Socket::listen(int backlog) {
    if (::listen(this->sock_fd_, backlog) < 0) {
        throw std::runtime_error(std::string("listen: ") +
                                 std::strerror(errno));
    }
}

int Socket::accept() {
    int client_fd = ::accept(this->sock_fd_, nullptr, nullptr);
    if (client_fd < 0) {
        throw std::runtime_error(std::string("accept: ") +
                                 std::strerror(errno));
    }

    return client_fd;
}

void Socket::connect_to(const struct sockaddr_in &addr) {
    if (::connect(this->sock_fd_,
                  reinterpret_cast<const struct sockaddr *>(&addr),
                  sizeof(addr)) < 0) {
        throw std::runtime_error(std::string("connect: ") +
                                 std::strerror(errno));
    }
}

ssize_t Socket::send_all(const std::string_view data) {
    ssize_t     total_sent = 0;
    const char *buf        = data.data();
    std::size_t left       = data.size();

    while (left > 0) {
        ssize_t bytes_sent = ::send(this->sock_fd_, buf + total_sent, left, 0);
        if (bytes_sent <= 0) {
            if (errno == EINTR) {
                continue; // Retry on interrupt
            }

            throw std::runtime_error(std::string("send: ") +
                                     std::strerror(errno));
        }

        total_sent += bytes_sent;
        left -= bytes_sent;
    }

    return total_sent;
}

std::string Socket::recv_line() {
    std::string output;
    char        ch;

    while (true) {
        ssize_t bytes_received = ::recv(this->sock_fd_, &ch, 1, 0);
        if (bytes_received < 0) {
            return std::string(); // Error occurred
        }

        output.push_back(ch);
        if (ch == '\n') {
            break; // End of line
        }
    }

    return output;
}

sockaddr_in Socket::make_addr(const std::string_view ip,
                              std::uint16_t          port) const {
    if (ip.empty()) {
        throw std::invalid_argument("make_addr: empty string");
    }

    struct sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = htons(port);

    int result = ::inet_aton(std::string(ip).c_str(), &sock_addr.sin_addr);
    if (!result) {
        throw std::invalid_argument(
            std::string("make_addr: inet_aton failed: ") +
            std::strerror(errno));
    }

    return sock_addr;
}

int Socket::make_socket(const struct sockaddr_in &addr) const {
    int sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        throw std::runtime_error(std::string("socket: ") +
                                 std::strerror(errno));
    }

    int opt = 1;
    if (::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0) {
        ::close(sock_fd);
        throw std::runtime_error(
            std::string("setsockopt SO_REUSEADDR failed: ") +
            std::strerror(errno));
    }

    if (::bind(sock_fd,
               reinterpret_cast<const struct sockaddr *>(&addr),
               sizeof(addr)) < 0) {
        ::close(sock_fd);
        throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
    }

    return sock_fd;
}

} // namespace core
