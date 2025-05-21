[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/-I_KKhDl)
# CSC4303 Assignment 5: JSON RPC Library (12 points)

### Deadline: April 15, 2025, 23:59

### Name:

### Student ID:

## Overview

In this assignment, you will implement a minimal custom Remote Procedure Call (RPC) library from scratch. The assignment involves using **JSON** for message formatting over TCP sockets. This includes implementing server and client stubs to enable communication between a C++ server and a Python client using JSON messages. Optional credits are given if you can optimize the RPC performance.

The goal is to understand the fundamental components of an RPC system (stubs, dispatch, network communication, error handling) using a common serialization format.

## Directory Structure
```
.
├── README.md # This file
├── rpc_lib/ # Core library components (You will implement parts here)
│ ├── cpp/ # C++ Server Library Parts
│ │ ├── include/
│ │ │ ├── rpc_protocol.h # Defines status codes, describes JSON format
│ │ │ ├── rpc_connection.h # Handles socket connection and length-prefixing
│ │ │ └── rpc_server_stub.h # Declares C++ RPCServer class (JSON-based)
│ │ └── src/
│ │ ├── rpc_connection.cpp # Implements Connection class
│ │ └── rpc_server_stub.cpp # Implements RPCServer class (JSON-based)
│ └── python/ # Python Client Library Parts
│ │ └── rpc_client_stub.py # Implements Python RPCClient class (JSON-based)
│ └──── rpc_exceptions.py # Defines custom Python exceptions
├── server_app/ # Example Server Application (Uses rpc_lib)
│ ├── Makefile
│ ├── main.cpp # Uses jsoncpp, registers functions with wrappers
│ ├── functions.h # Declares C++ functions with basic types
│ └── functions.cpp # Implements C++ functions with basic types
└── client_app/ # Example Client Application (Uses rpc_lib)
└── main.py # Uses json module
```

## Task: Implementation (9 points)

Your primary goal is to implement the core JSON-based RPC components. Each file contains detailed TODO comments to guide your implementation.

1. **Understand the JSON Protocol**
   * Familiarize yourself with the JSON request/response formats defined in `rpc_protocol.h`
   * Request: `{"function": "<name>", "args": [arg1, arg2, ...]}`
   * Response (Success): `{"status": "success", "result": <value>}`
   * Response (Error): `{"status": "error", "message": "<error_string>"}`

2. **Implement C++ Connection Handling (2 points)**
   * Complete the `rpc_connection.cpp` file with socket I/O functionality
   * Implement data sending/receiving with proper error handling
   * Handle connection lifecycle (opening, closing, state management)

3. **Implement C++ Server Stub (3 points)**
   * Complete the `rpc_server_stub.cpp` file to process RPC requests
   * Implement function registration, lookup, and invocation
   * Handle JSON parsing/generation and error conditions

4. **Implement Python Client Stub (3 points)**
   * Complete the `rpc_client_stub.py` file to make RPC calls
   * Implement JSON serialization/deserialization of requests/responses
   * Handle socket communication and error propagation

5. **Implement the Server Application (1 points)**
   * Complete the TODOs in `server_app/main.cpp` 
   * Implement function registrations

## Task: Report (3 points)
6. Submit a `report.md` file describing your understanding of the JSON RPC system design (2 points) and presenting benchmark results obtained by running `benchmark.py` (1 point).

## Building and Testing

* **Dependency:** Install the `jsoncpp` development library: `sudo apt-get install libjsoncpp-dev`
* Build the server: `cd server_app && make`
* Run the server: `./server_app/server <port>`
* Run the client: `python client_app/main.py localhost <port>`
* Run the test:   `python client_app/test.py 127.0.0.1 <port>`
* Run benchmarks: `python client_app/benchmark.py localhost <port>`

## Bonus: Optimizing the RPC System (3 points)

For extra credit, implement and evaluate performance optimizations for your RPC system. Some ideas:

* Replace JSON with a binary protocol for more efficient serialization
* Optimize function dispatch with faster lookup mechanisms
* Implement other performance improvements

Requirements:
* Create a `/bonus` directory containing all code for your optimized system. This should be a separate implementation; do not modify your original submission.
* The `/bonus` directory must also include a `report.md` file detailing your optimization strategies and presenting benchmark results obtained by running `benchmark.py` comparing the original and optimized systems.
* Instructions for running your optimized RPC system must also be included in the `report.md`.
* Bonus points will be added to your assignment score, but the total score for all assignments (including bonus points) cannot exceed 60 points (60% of the final grade).

---
Have fun!

Author: Juan Albert Wibowo
