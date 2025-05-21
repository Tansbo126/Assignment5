#include "../include/rpc_connection.h"
#include <unistd.h> // For close(), read(), write()
#include <sys/socket.h> // For send(), recv()
#include <arpa/inet.h> // For ntohl()
#include <stdexcept>
#include <cstring> // For strerror
#include <cerrno>  // For errno
#include <iostream> // For error reporting

namespace rpc_connection {

// --- Constructor / Destructor ---

// We provide the constructors and destructors as helper code
Connection::Connection(int socket_fd) : fd(socket_fd), open(socket_fd >= 0) {
    if (!open) {
        // Optional: Log or handle invalid fd case
        // For now, just mark as not open.
    }
    // std::cout << "Connection created with fd: " << fd << std::endl;
}

Connection::~Connection() {
    // std::cout << "Connection destructor called for fd: " << fd << std::endl;
    if (open) {
        close_connection();
    }
}

// --- Move Semantics ---

// We provide move semantics as helper code
Connection::Connection(Connection&& other) noexcept : fd(other.fd), open(other.open) {
    // Reset the other object to prevent double-closing
    other.fd = -1;
    other.open = false;
    // std::cout << "Connection move constructor called, new fd: " << fd << std::endl;
}

Connection& Connection::operator=(Connection&& other) noexcept {
    if (this != &other) {
        // Close the current connection if it's open
        if (open) {
            close_connection();
        }
        // Transfer ownership
        fd = other.fd;
        open = other.open;
        // Reset the other object
        other.fd = -1;
        other.open = false;
        // std::cout << "Connection move assignment called, new fd: " << fd << std::endl;
    }
    return *this;
}

// --- Core I/O Operations ---

/**
 * TODO: Implement the send_data method
 * 
 * This method sends data over the connection.
 * Key concepts to implement:
 * 1. Check if the connection is open
 * 2. Send all bytes in the data vector
 * 3. Handle partial sends and retry as needed
 * 4. Handle errors appropriately (including EINTR interruptions)
 * 5. Mark connection as closed and throw ConnectionError on failure
 * 
 * @param data The vector of bytes to send
 * @throws ConnectionError if there's an error or the connection is closed
 */
void Connection::send_data(const std::vector<uint8_t>& data) {
    if (!open) throw ConnectionError("Connection is closed");
    
    size_t total_sent = 0;
    while (total_sent < data.size()) {
        ssize_t sent = send(fd, data.data() + total_sent, data.size() - total_sent, 0);
        if (sent < 0) {
            if (errno == EINTR) continue;
            open = false;
            throw ConnectionError("Send error: " + std::string(strerror(errno)));
        }
        total_sent += sent;
    }
}

/**
 * TODO: Implement the receive_data method
 * 
 * This method receives a specific number of bytes from the connection.
 * Key concepts to implement:
 * 1. Check if the connection is open
 * 2. Allocate a buffer of the requested size
 * 3. Receive exactly the requested number of bytes
 * 4. Handle partial receives and retry as needed
 * 5. Handle errors and connection closures
 * 6. Return received data as a vector
 * 
 * @param num_bytes The number of bytes to receive
 * @return A vector containing the received bytes
 * @throws ConnectionError if there's an error or the connection is closed
 */
std::vector<uint8_t> Connection::receive_data(size_t num_bytes) {
    if (!open) throw ConnectionError("Connection is closed");
    
    std::vector<uint8_t> buffer(num_bytes);
    size_t total_received = 0;
    while (total_received < num_bytes) {
        ssize_t received = recv(fd, buffer.data() + total_received, num_bytes - total_received, 0);
        if (received == 0) {
            open = false;
            throw ConnectionError("Connection closed by peer");
        }
        if (received < 0) {
            if (errno == EINTR) continue;
            open = false;
            throw ConnectionError("Receive error: " + std::string(strerror(errno)));
        }
        total_received += received;
    }
    return buffer;
}
/**
 * TODO: Implement the receive_length_prefix method
 * 
 * This method receives a 4-byte length prefix and converts it from network byte order.
 * Key concepts to implement:
 * 1. Receive exactly 4 bytes using receive_data
 * 2. Convert from network byte order (big-endian) to host byte order using ntohl
 * 
 * @return The length value in host byte order
 * @throws ConnectionError if there's an error or the connection is closed
 */
uint32_t Connection::receive_length_prefix() {
    auto data = receive_data(4);
    uint32_t length;
    memcpy(&length, data.data(), 4);
    return ntohl(length);
}

/**
 * TODO: Implement the close_connection method
 * 
 * This method closes the socket and marks the connection as closed.
 * Key concepts to implement:
 * 1. Check if the connection is already closed
 * 2. Close the socket file descriptor using close()
 * 3. Update the state variables (fd and open) to reflect the closed connection
 * 4. Handle any errors from close() (log but don't throw)
 */
void Connection::close_connection() {
    if (!open) return;
    open = false;
    if (close(fd) < 0) {
        std::cerr << "Close error: " << strerror(errno) << std::endl;
    }
    fd = -1;
}

/**
 * TODO: Implement the is_open method
 * 
 * This method returns whether the connection is currently open.
 * 
 * @return true if the connection is open, false otherwise
 */
bool Connection::is_open() const {
    return open;
}
} // namespace rpc_connection
