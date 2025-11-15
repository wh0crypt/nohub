//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file server.h
/// Server interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#ifndef NOHUB_CORE_SERVER_H
#define NOHUB_CORE_SERVER_H

#include "socket.h"

#include <atomic>
#include <mutex>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace core {

class Server {
  public:
    /// \brief Constructor for Server class.
    ///
    /// \param port Port number to bind the server socket.
    /// \throws std::runtime_error if socket creation or binding fails.
    explicit Server(std::uint16_t port);
    Server() = delete;

    /// \brief Destructor for Server class.
    ///
    /// Stops the server if it is running.
    ~Server();

    /// \brief Run the server to accept incoming client connections.
    void run() noexcept;

    /// \brief Stop the server and disconnect all clients.
    void stop() noexcept;

  private:
    /// Accept loop to handle incoming client connections.
    ///
    /// \throws std::runtime_error if accepting a connection fails.
    void accept_loop();

    /// Client handling loop.
    ///
    /// \param client_sock_fd The socket file descriptor of the connected
    /// client.
    void client_loop(int client_sock_fd) noexcept;

    /// Broadcast a message to all connected clients.
    ///
    /// \param message The message to broadcast.
    /// \param exclude_sock_fd The socket file descriptor to exclude from
    /// broadcasting (default is -1, meaning no exclusion).
    void broadcast(const std::string_view message,
                   int                    exclude_sock_fd = -1) noexcept;

    Socket                               server_socket_;
    std::atomic<bool>                    is_running_;
    std::mutex                           clients_mutex_;
    std::vector<int>                     client_sock_fds_;
    std::unordered_map<int, std::thread> client_threads_;
};

} // namespace core

#endif // NOHUB_CORE_SERVER_H
