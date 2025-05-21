#ifndef RPC_SERVER_STUB_H
#define RPC_SERVER_STUB_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory> // For std::unique_ptr
#include <cstdint>
#include <json/json.h> // Include jsoncpp header
#include "rpc_protocol.h"
#include "rpc_connection.h" // Connection class is defined here

namespace rpc_server {

// Define the signature for functions that can be registered with the server.
// For the JSON version, functions take a Json::Value (representing args array or object)
// and return a Json::Value (representing the result or error details).
using RPCFunction = std::function<Json::Value(const Json::Value&)>;

/**
 * @brief The main RPC server class. Handles listening for connections,
 * receiving requests, dispatching calls to registered functions,
 * and sending responses.
 */
class RPCServer {
public:
    /**
     * @brief Constructs the RPC server to listen on the specified port.
     * @param port The port number to listen on.
     */
    explicit RPCServer(int port);

    /**
     * @brief Destructor. Stops the server if running.
     */
    virtual ~RPCServer();

    // Disable copy constructor and assignment operator
    RPCServer(const RPCServer&) = delete;
    RPCServer& operator=(const RPCServer&) = delete;

    /**
     * @brief Registers a function that can be called remotely.
     * @param name The name used by the client to call the function.
     * @param func The function implementation matching the RPCFunction signature.
     */
    void register_function(const std::string& name, RPCFunction func);

    /**
     * @brief Starts the server's main listening loop.
     * This function will block until stop() is called or an error occurs.
     * It sets up the listening socket and enters the accept loop.
     * @throws std::runtime_error if server setup fails (e.g., socket, bind, listen).
     */
    void start();

    /**
     * @brief Stops the server's listening loop.
     */
    void stop();

private:
    /**
     * @brief Handles communication with a single connected client.
     * Reads requests, processes them, sends responses, until the client
     * disconnects or an error occurs.
     * @param connection A unique_ptr to the Connection object for the client.
     */
    void handle_client(std::unique_ptr<Connection> connection);

    /**
     * @brief Processes a single RPC request received from a client.
     * Unmarshals the request, finds the function, executes it, and marshals the response.
     * @param request_json The raw bytes of the request (after length prefix).
     * @return A byte vector containing the length-prefixed JSON response string.
     */
    std::vector<uint8_t> process_json_request(const std::string& request_json);

    /**
     * @brief Creates and prepares the listening socket.
     * @return The file descriptor for the listening socket.
     * @throws std::runtime_error on failure.
     */
    int setup_listening_socket();

    int port;
    int listen_fd = -1; // Listening socket file descriptor
    std::map<std::string, RPCFunction> function_registry;
    bool running = false;
};

} // namespace rpc_server

#endif // RPC_SERVER_STUB_H
