//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file client.cpp
/// Client interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#include "client.h"

#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>

namespace core {

Client::Client(const std::string_view server_address,
               std::uint16_t          server_port) {
    this->socket_ = Socket::create_tcp_socket();
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(server_port);
    if (inet_pton(AF_INET,
                  std::string(server_address).c_str(),
                  &server_addr.sin_addr) <= 0) {
        throw std::invalid_argument(
            "client constructor: invalid server ip address");
    }

    this->socket_.connect_to(server_addr);
}

void Client::run_interactive() {
    this->reader_thread_ = std::thread([this]() {
        try {
            while (true) {
                std::string message = this->socket_.recv_line();
                if (message.empty()) {
                    break; // Server disconnected
                }

                std::printf("%s", message.c_str());
            }
        } catch (const std::exception &e) {
            std::fprintf(stderr, "[-] reader_thread: %s\n", e.what());
        }
    });

    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "/quit") {
            break;
        }

        input.push_back('\n');
        if (this->socket_.send_all(input) < 0) {
            std::fprintf(stderr, "[-] Failed to send message to server.\n");
            break;
        }
    }

    ::shutdown(this->socket_.sock_fd(), SHUT_RDWR);
    if (this->reader_thread_.joinable()) {
        this->reader_thread_.join();
    }
}

} // namespace core
