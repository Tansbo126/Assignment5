#ifndef RPC_PROTOCOL_H
#define RPC_PROTOCOL_H

#include <cstdint>
#include <string>

namespace rpc_protocol {

// --- JSON Message Structure ---
// All messages are sent as UTF-8 encoded JSON strings.
// Each JSON string is prefixed by a 4-byte network byte order (big-endian)
// unsigned integer representing the length of the JSON string.

/*
Request JSON Format:
{
  "function": "<function_name_string>",
  "args": [ <arg1_json_value>, <arg2_json_value>, ... ]
}
- "args" is a JSON array containing arguments. Arguments can be JSON numbers
  (int or float), strings, booleans, or null.
*/

/*
Response JSON Format (Success):
{
  "status": "success",
  "result": <return_value_json>
}
- "result" can be any valid JSON value (number, string, boolean, null, array, object).
*/

/*
Response JSON Format (Error):
{
  "status": "error",
  "message": "<error_description_string>"
}
*/


// --- Status Codes ---
// These are used internally by the server/client stubs but are also
// reflected in the "status" field of the JSON response.

// TODO: Student should define appropriate string constants or use these enums
//       when constructing/parsing the "status" field in JSON.
enum class StatusCode : uint8_t {
    RPC_SUCCESS         = 0x00, // Corresponds to "success" status string
    RPC_ERROR_FUNCTION_NOT_FOUND = 0x01, // Corresponds to "error" status string
    RPC_ERROR_INVALID_ARGS   = 0x02, // Corresponds to "error" status string (Example: wrong type/number)
    RPC_ERROR_EXECUTION_FAILED   = 0x03, // Corresponds to "error" status string
    RPC_ERROR_JSON_PARSE_FAILED  = 0x04, // Corresponds to "error" status string
    RPC_ERROR_INTERNAL           = 0x05  // Corresponds to "error" status string
    // Add more specific error codes if needed
};

// Helper function to map StatusCode enum to JSON status string
inline std::string status_to_string(StatusCode code) {
    switch (code) {
        case StatusCode::RPC_SUCCESS: return "success";
        default: return "error";
    }
}

} // namespace rpc_protocol

#endif // RPC_PROTOCOL_H
