#include "../include/rpc_server_stub.h"
#include <jsoncpp/json/json.h>  // For JSON parsing/serialization
#include <sys/socket.h> // For socket, bind, listen, accept
#include <netinet/in.h> // For sockaddr_in, INADDR_ANY, htonl, htons
#include <arpa/inet.h>  // For htonl, ntohl (needed for length prefix)
#include <unistd.h>     // For close
#include <stdexcept>
#include <iostream>
#include <cstring>      // For strerror, memset
#include <cerrno>       // For errno
#include <memory>       // For std::unique_ptr

namespace rpc_server {

using namespace rpc_protocol;
using namespace rpc_connection;

// --- Constructor / Destructor ---

RPCServer::RPCServer(int port) : port(port), listen_fd(-1), running(false) {
    if (port <= 0 || port > 65535) {
        throw std::invalid_argument("Invalid port number");
    }
}

RPCServer::~RPCServer() {
    stop(); // Ensure server is stopped and socket is closed
}

// --- Public Methods ---

/**
 * TODO: Implement the register_function method
 * 
 * This method allows clients to register functions that can be called remotely.
 * Your implementation should:
 * 1. Check if server is already running (should not register while running)
 * 2. Check if function with the same name already exists (print warning)
 * 3. Store the function in the function_registry map
 * 4. Print confirmation message
 * 
 * @param name The name of the function to register
 * @param func The function to be registered
 */
void RPCServer::register_function(const std::string& name, RPCFunction func) {
    if (running) throw std::runtime_error("Cannot register functions while server is running");
    if (function_registry.count(name)) {
        std::cerr << "Warning: Function '" << name << "' already registered" << std::endl;
        return;
    }
    function_registry[name] = func;
    std::cout << "Registered function: " << name << std::endl;
}

/**
 * TODO: Implement the start method
 * 
 * This method starts the RPC server and listens for incoming connections.
 * Your implementation should:
 * 1. Check if server is already running
 * 2. Set up a listening socket using setup_listening_socket()
 * 3. Enter an accept loop to handle client connections
 * 4. For each connection, create a Connection object and handle client requests
 * 5. Handle errors with try/catch blocks
 * 6. Clean up resources when server is stopped
 */
void RPCServer::start() {
    if (running) return;
    listen_fd = setup_listening_socket();
    running = true;
    std::cout << "Server listening on port " << port << std::endl;
    
    while (running) {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (!running) break;
            std::cerr << "Accept error: " << strerror(errno) << std::endl;
            continue;
        }
        std::thread([this, client_fd]() {
            auto conn = std::make_unique<Connection>(client_fd);
            try {
                handle_client(std::move(conn));
            } catch (const std::exception& e) {
                std::cerr << "Client handler error: " << e.what() << std::endl;
            }
        }).detach();
    }
    close(listen_fd);
}

/**
 * TODO: Implement the stop method
 * 
 * This method stops the RPC server.
 * Your implementation should:
 * 1. Check if server is already stopped
 * 2. Signal the server to stop accepting new connections
 * 3. Close the listening socket (use shutdown() and close())
 * 4. Update server state
 */
void RPCServer::stop() {
    if (!running) return;
    running = false;
    shutdown(listen_fd, SHUT_RDWR);
    close(listen_fd);
    std::cout << "Server stopped" << std::endl;
}
// --- Private Helper Methods ---

// We provide this helper method to handle socket setup
int RPCServer::setup_listening_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
    }

    // Allow reusing the address (helpful for quick restarts)
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        close(sockfd);
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)));
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
    }

    // Start listening, allow a backlog of connections (e.g., 10)
    if (listen(sockfd, 10) < 0) {
        close(sockfd);
        throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
    }

    return sockfd;
}

/**
 * TODO: Implement the handle_client method
 * 
 * This method handles communication with a connected client.
 * Your implementation should:
 * 1. Enter a loop to process client requests while connection is open
 * 2. Read the length prefix and request payload
 * 3. Process the request using process_json_request
 * 4. Send the response back to the client
 * 5. Handle connection errors and processing errors appropriately
 * 6. Send error responses to the client when possible
 * 
 * @param connection A unique_ptr to the Connection object for this client
 */
void RPCServer::handle_client(std::unique_ptr<Connection> connection) {
    try {
        while (connection->is_open()) {
            uint32_t length = connection->receive_length_prefix();
            auto data = connection->receive_data(length);
            std::string request(data.begin(), data.end());
            auto response = process_json_request(request);
            connection->send_data(response);
        }
    } catch (const ConnectionError& e) {
        std::cerr << "Client disconnected: " << e.what() << std::endl;
    }
}
/**
 * TODO: Implement the process_json_request method
 * 
 * This method processes a JSON-encoded RPC request and produces a response.
 * Your implementation should:
 * 1. Parse the incoming JSON string using jsoncpp
 * 2. Validate the request format has "function" and "args" fields
 * 3. Extract the function name and arguments
 * 4. Look up the function in the registry
 * 5. Execute the function with the provided arguments
 * 6. Build a response JSON with the result
 * 7. Handle errors at each stage and create appropriate error responses
 * 8. Serialize the response JSON and add length prefix
 * 
 * @param request_json_str The JSON-encoded RPC request
 * @return A vector of bytes containing the length-prefixed JSON response
 */
std::vector<uint8_t> RPCServer::process_json_request(const std::string& request_json_str) {
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream iss(request_json_str);
    
    if (!Json::parseFromStream(reader, iss, &root, &errors)) {
        Json::Value error;
        error["status"] = "error";
        error["message"] = "Invalid JSON: " + errors;
        return serialize_response(error);
    }
    
    if (!root.isMember("function") || !root.isMember("args")) {
        Json::Value error;
        error["status"] = "error";
        error["message"] = "Missing 'function' or 'args' field";
        return serialize_response(error);
    }
    
    std::string func_name = root["function"].asString();
    Json::Value args = root["args"];
    
    try {
        if (!function_registry.count(func_name)) {
            throw FunctionNotFoundError("Function not found");
        }
        
        auto result = function_registry[func_name](args);
        Json::Value response;
        response["status"] = "success";
        response["result"] = result;
        return serialize_response(response);
    } catch (const FunctionNotFoundError& e) {
        Json::Value error;
        error["status"] = "error";
        error["message"] = e.what();
        return serialize_response(error);
    } catch (const std::exception& e) {
        Json::Value error;
        error["status"] = "error";
        error["message"] = "Execution error: " + std::string(e.what());
        return serialize_response(error);
    }
}