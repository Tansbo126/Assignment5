import socket
import struct
import json  # Use standard json module
from typing import Any, List, Tuple, Union

# Import only exceptions, not marshal functions
# Use absolute import because rpc_lib/python is added to sys.path
from rpc_exceptions import (
    ConnectionError,
    RPCError,
    FunctionNotFoundError,
    ExecutionError,
    ProtocolError,
    MarshalingError,  # Keep for JSON encoding/decoding issues
)

# Type alias for RPC values in Python (can be any JSON-serializable type)
RPCValue = Union[int, float, str, bool, None, List[Any], dict]


class RPCClient:
    """Client stub for making RPC calls to a C++ server using JSON."""

    def __init__(self, host: str, port: int):
        """
        Initializes the RPC client but does not connect yet.
        Args:
            host: The hostname or IP address of the server.
            port: The port number of the server.
        """
        self.host = host
        self.port = port
        self.sock: Union[socket.socket, None] = None
        self._is_connected = False

    def connect(self):
        """
        TODO: Establish a connection to the RPC server.

        Your implementation should:
        1. Check if already connected
        2. Create a new socket
        3. Connect to the server using the host and port
        4. Handle any socket errors appropriately
        5. Update the connection state
        """
        # Your implementation here
        if self._is_connected:
            return
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect((self.host, self.port))
            self._is_connected = True
        except socket.error as e:
            self._handle_socket_error(e, "connect")
        pass

    def disconnect(self):
        """
        TODO: Close the connection to the RPC server.

        Your implementation should:
        1. Check if already disconnected
        2. Properly shutdown the socket
        3. Close the socket
        4. Handle any socket errors
        5. Update the connection state
        """
        # Your implementation here
        if not self._is_connected:
            return
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
            self.sock.close()
        except socket.error as e:
            pass
        finally:
            self.sock = None
            self._is_connected = False
        pass

    def _send_all(self, data: bytes):
        """
        Helper method to send all data reliably.

        Your implementation should:
        1. Check if connected
        2. Use sock.sendall() to send data
        3. Handle socket errors and timeouts
        """
        # Your implementation here
        if not self._is_connected:
            raise ConnectionError("Not connected")
        try:
            self.sock.sendall(data)
        except socket.error as e:
            self._handle_socket_error(e, "send")
        pass

    def _recv_all(self, num_bytes: int) -> bytes:
        """
        Helper method to receive an exact number of bytes reliably.

        Your implementation should:
        1. Check if connected
        2. Create a buffer to store received data
        3. Loop until all requested bytes are received
        4. Handle partial receives, socket errors, and timeouts
        5. Return the complete received data
        """
        # Your implementation here
        if not self._is_connected:
            raise ConnectionError("Not connected")
        data = bytearray()
        while len(data) < num_bytes:
            chunk = self.sock.recv(num_bytes - len(data))
            if not chunk:
                self._handle_socket_error(ConnectionError("Connection closed"), "recv")
            data.extend(chunk)
        return bytes(data)
        pass

    def _handle_socket_error(self, e: Exception, operation: str):
        """
        Handles socket errors, updates connection state, and raises ConnectionError.

        Your implementation should:
        1. Disconnect the socket
        2. Raise a ConnectionError with appropriate message
        """
        # Your implementation here
        self.disconnect()
        raise ConnectionError(f"Socket error during {operation}: {str(e)}") from e
        
def call(self, func_name: str, *args: RPCValue) -> RPCValue:
        """
        TODO: Make an RPC call to the server using JSON.

        This is the core RPC method that implements the client-side protocol.
        Your implementation should:
        1. Check if connected
        2. Construct a JSON request with the function name and arguments
        3. Send the request with appropriate length prefix
        4. Receive the response with length prefix
        5. Parse and validate the JSON response
        6. Handle different status codes (success, error)
        7. Return the result or raise appropriate exceptions

        Args:
            func_name: The name of the function to call.
            *args: The arguments to pass to the function (must be JSON-serializable).

        Returns:
            The return value from the remote function.

        Raises:
            ConnectionError: If not connected or connection fails during call.
            MarshalingError: If request/response JSON encoding/decoding fails.
            ProtocolError: If the server response violates the expected JSON structure.
            FunctionNotFoundError: If the server indicates the function doesn't exist.
            ExecutionError: If the server indicates an error during function execution.
            RPCError: For other RPC-related errors.
        """
        # Your implementation here
        if not self._is_connected:
            raise ConnectionError("Not connected")
        
        request = {
            "function": func_name,
            "args": list(args)
        }
        try:
            request_json = json.dumps(request).encode('utf-8')
        except TypeError as e:
            raise MarshalingError(f"JSON encoding failed: {e}") from e
        
        try:
            length_prefix = struct.pack('!I', len(request_json))
            self._send_all(length_prefix + request_json)
            
            length_bytes = self._recv_all(4)
            response_length = struct.unpack('!I', length_bytes)[0]
            response_json = self._recv_all(response_length).decode('utf-8')
            
            response = json.loads(response_json)
            if response.get('status') == 'success':
                return response.get('result', None)
            else:
                message = response.get('message', 'Unknown error')
                if "Function not found" in message:
                    raise FunctionNotFoundError(message)
                elif "Execution error" in message:
                    raise ExecutionError(message)
                else:
                    raise RPCError(message)
        except json.JSONDecodeError as e:
            raise ProtocolError(f"Invalid JSON response: {e}") from e
        pass

    # Context manager support - we provide these for convenience
def __enter__(self):
        """Enter the runtime context related to this object."""
        self.connect()
        return self

def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the runtime context related to this object."""
        self.disconnect()
