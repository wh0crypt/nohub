//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file client.h
/// Client interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#ifndef NOHUB_CORE_CLIENT_H
#define NOHUB_CORE_CLIENT_H

#include "socket.h"

#include <thread>

namespace core {

class Client {
  public:
    /// \brief Constructor for Client class.
    ///
    /// \param server_address The server address to connect to.
    /// \param server_port The server port to connect to.
    /// \throws std::invalid_argument if the server IP address is invalid.
    explicit Client(const std::string_view server_address,
                    std::uint16_t          server_port);
    Client() = delete;

    void run_interactive();

  private:
    Socket      socket_;
    std::thread reader_thread_;
};

} // namespace core

#endif // NOHUB_CORE_CLIENT_H
