#ifndef RPC_CONNECTION_H
#define RPC_CONNECTION_H

#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept> // For std::runtime_error

namespace rpc_connection {

// Exception class for connection errors
class ConnectionError : public std::runtime_error {
public:
    ConnectionError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Represents a network connection (e.g., a TCP socket).
 * This class encapsulates basic read/write operations.
 */
class Connection {
public:
    // Constructor takes an existing socket file descriptor
    Connection(int socket_fd);

    // Destructor closes the connection
    virtual ~Connection();

    // Disable copy constructor and assignment operator
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    // Move constructor and assignment operator (optional but good practice)
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;

    /**
     * @brief Sends a block of data over the connection.
     * @param data The byte vector to send.
     * @throws ConnectionError if sending fails.
     */
    void send_data(const std::vector<uint8_t>& data);

    /**
     * @brief Receives a block of data of a specific size.
     * Blocks until the specified number of bytes are received.
     * @param num_bytes The exact number of bytes to receive.
     * @return A byte vector containing the received data.
     * @throws ConnectionError if receiving fails or connection is closed prematurely.
     */
    std::vector<uint8_t> receive_data(size_t num_bytes);

    /**
     * @brief Receives the 4-byte length prefix from the stream.
     * Used to determine the size of the incoming message payload.
     * @return The length of the expected payload in host byte order.
     * @throws ConnectionError if receiving fails or connection is closed.
     */
    uint32_t receive_length_prefix();

    /**
     * @brief Closes the connection.
     */
    void close_connection();

    /**
     * @brief Checks if the connection is currently open.
     * @return True if the connection is open, false otherwise.
     */
    bool is_open() const;

private:
    int fd = -1; // Socket file descriptor
    bool open = false;
};

} // namespace rpc_connection

#endif // RPC_CONNECTION_H
