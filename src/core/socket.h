//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file socket.h
/// Socket interface for the NoHub project.
///
//===----------------------------------------------------------------------===//

#ifndef NOHUB_CORE_SOCKET_H
#define NOHUB_CORE_SOCKET_H

#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>

namespace core {

class Socket {
  public:
    /// \brief Constructor for Socket class.
    ///
    /// \param sock_fd Socket file descriptor.
    explicit Socket(int sock_fd = -1) noexcept;

    /// \brief Constructor for Socket class from IP and port.
    ///
    /// \param ip IP address as a string view.
    /// \param port Port number.
    /// \throws std::invalid_argument if IP address or port is invalid.
    explicit Socket(const std::string_view ip, std::uint16_t port);

    /// \brief Constructor for Socket class from sockaddr_in.
    ///
    /// \param addr Socket address structure.
    /// \throws std::runtime_error if socket creation or binding fails.
    explicit Socket(const struct sockaddr_in &addr);

    /// \brief Delete copy constructor and copy assignment operator.
    Socket(const Socket &)            = delete;
    Socket &operator=(const Socket &) = delete;

    /// \brief Move constructor.
    ///
    /// \param other Rvalue reference to another Socket object.
    Socket(Socket &&other) noexcept;

    /// \brief Move assignment operator.
    ///
    /// \param other Rvalue reference to another Socket object.
    /// \return Reference to this Socket object.
    Socket &operator=(Socket &&other) noexcept;

    /// \brief Create a TCP socket.
    ///
    /// \return Socket object representing the created TCP socket.
    /// \throws std::runtime_error if socket creation fails.
    static Socket create_tcp_socket();

    /// \brief Destructor for Socket class.
    ~Socket();

    /// \brief Get the socket file descriptor.
    ///
    /// \return Socket file descriptor.
    int sock_fd() const noexcept;

    /// \brief Convert a sockaddr_in structure to a human-readable string.
    ///
    /// \return String representation of the IP address and port.
    std::string addr_str() const;

    /// \brief Start listening for incoming connections.
    ///
    /// \param backlog Maximum length of the queue of pending connections.
    /// \throws std::runtime_error if listen fails.
    void listen(int backlog = SOMAXCONN);

    /// \brief Accept an incoming connection.
    ///
    /// \return Socket file descriptor for the accepted connection.
    /// \throws std::runtime_error if accept fails.
    int accept();

    /// \brief Connect to a remote address.
    ///
    /// \param addr sockaddr_in structure representing the remote address.
    /// \throws std::runtime_error if connect fails.
    void connect_to(const struct sockaddr_in &addr);

    /// \brief Bind the socket to a specific address.
    ///
    /// \param addr sockaddr_in structure representing the address to bind to.
    /// \throws std::runtime_error if bind fails.
    void bind_to(const struct sockaddr_in &addr);

    /// \brief Send all data over the socket.
    ///
    /// \param data Data to send as a string view.
    /// \return Number of bytes sent.
    ssize_t send_all(const std::string_view data);

    /// \brief Receive a line of data from the socket.
    ///
    /// \return Received line as a string or an empty string on error.
    std::string recv_line();

  private:
    /// \brief Socket address structure.
    struct sockaddr_in addr_;

    /// \brief Socket file descriptor.
    int sock_fd_;

    /// \brief Create a sockaddr_in structure from an IP address and port.
    ///
    /// \param ip IP address as a string view.
    /// \param port Port number.
    /// \return sockaddr_in structure representing the IP address and port.
    /// \throws std::invalid_argument if the IP address or port is invalid.
    sockaddr_in make_addr(const std::string_view ip, std::uint16_t port) const;

    /// \brief Create a socket and return its file descriptor.
    ///
    /// \param addr sockaddr_in structure for the socket.
    /// \return Socket file descriptor.
    /// \throws std::runtime_error if socket creation fails.
    int make_socket(const struct sockaddr_in &addr) const;
};

} // namespace core

#endif // NOHUB_CORE_SOCKET_H
