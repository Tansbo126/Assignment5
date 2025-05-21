#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector> // Needed for std::vector access in lambdas
#include <jsoncpp/json/json.h> // Needed for Json::Value

#include "../rpc_lib/cpp/include/rpc_server_stub.h"
#include "functions.h" // Implementations of the functions we'll register

using namespace rpc_server;
using namespace rpc_protocol; // For StatusCode enum

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>" << std::endl;
        return 1;
    }

    int port;
    try {
        port = std::stoi(argv[1]);
        if (port <= 0 || port > 65535) {
            throw std::invalid_argument("Port number must be between 0 and 65535");
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid port number: " << e.what() << std::endl;
        return 1;
    }

    try {
        RPCServer server(port);

        // --- Basic Data Type Function Registrations ---
        server.register_function("add", [](const Json::Value& args) -> Json::Value {
            // Implementation wrapper for add_impl is provided as reference
            // 1. Validate args: should be a JSON array of exactly two integers.
            // 2. Extract integer arguments.
            // 3. Call add_impl and return the result as Json::Value.
            if (!args.isArray() || args.size() != 2 || !args[0].isInt() || !args[1].isInt()) {
                throw std::invalid_argument("add requires two integer arguments");
            }
            // Extract args from JSON, call implementation, convert result to JSON
            int32_t result = add_impl(args[0].asInt(), args[1].asInt());
            return Json::Value(result);
        });

        server.register_function("greet", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for greet_impl
            // 1. Validate args: should be a JSON array containing one string.
            // 2. Extract string argument.
            // 3. Call greet_impl and return the result as Json::Value.
            if (!args.isArray() || args.size() != 1 || !args[0].isString()) {
                throw std::invalid_argument("greet requires one string argument");
            }
            return Json::Value(greet_impl(args[0].asString()));
        });

        server.register_function("is_positive", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for is_positive_impl
            // 1. Validate args: should be a JSON array containing one numeric value.
            // 2. Extract numeric argument.
            // 3. Call is_positive_impl and return the boolean result as Json::Value.
            if (!args.isArray() || args.size() != 1 || !args[0].isNumeric()) {
                throw std::invalid_argument("is_positive requires one numeric argument");
            }
            return Json::Value(is_positive_impl(args[0].asFloat()));
        });

        server.register_function("echo", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement echo function
            // 1. Validate args: should be a JSON array containing exactly one argument.
            // 2. Return the first argument directly.
            if (!args.isArray() || args.size() != 1) {
                throw std::invalid_argument("echo requires exactly one argument");
            }
            return args[0];
        });

        server.register_function("no_return", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for no_return_impl
            // 1. Validate args: should be an empty JSON array.
            // 2. Call no_return_impl.
            // 3. Return JSON null.
            if (!args.isArray() || !args.empty()) {
                throw std::invalid_argument("no_return takes no arguments");
            }
            no_return_impl();
            return Json::Value();
        });

        server.register_function("divide", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for divide_impl
            // 1. Validate args: should be a JSON array containing exactly two integers.
            // 2. Extract integer arguments.
            // 3. Handle division by zero exception.
            // 4. Call divide_impl and return the result as Json::Value.
            if (!args.isArray() || args.size() != 2 || !args[0].isInt() || !args[1].isInt()) {
                throw std::invalid_argument("divide requires two integers");
            }
            int32_t denominator = args[1].asInt();
            if (denominator == 0) {
                throw std::runtime_error("Division by zero");
            }
            return Json::Value(divide_impl(args[0].asInt(), denominator));
        });

        // --- Complex Data Type Function Registrations ---

        server.register_function("sum_array", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for sum_array_impl
            // 1. Validate args: should be a JSON array containing one element, which is an array of integers.
            // 2. Extract numbers from the inner JSON array into a std::vector<int32_t>.
            // 3. Call sum_array_impl with the vector.
            // 4. Convert the integer result back to Json::Value.
            if (!args.isArray() || args.size() != 1 || !args[0].isArray()) {
                throw std::invalid_argument("sum_array requires one array of integers");
            }
            std::vector<int32_t> numbers;
            for (const auto& item : args[0]) {
                if (!item.isInt()) {
                    throw std::invalid_argument("All array elements must be integers");
                }
                numbers.push_back(item.asInt());
            }
            return Json::Value(sum_array_impl(numbers));
        });

        server.register_function("process_person", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for process_person_impl
            // 1. Validate args: should be a JSON array containing one element, which is an object.
            // 2. Validate the object has "name" (string), "age" (int), "is_student" (bool) fields.
            // 3. Create a Person struct from the JSON object.
            // 4. Call process_person_impl with the struct.
            // 5. Convert the string result back to Json::Value.
            if (!args.isArray() || args.size() != 1 || !args[0].isObject()) {
                throw std::invalid_argument("process_person requires one object");
            }
            const Json::Value& obj = args[0];
            if (!obj.isMember("name") || !obj["name"].isString() ||
                !obj.isMember("age") || !obj["age"].isInt() ||
                !obj.isMember("is_student") || !obj["is_student"].isBool()) {
                throw std::invalid_argument("Person object requires name (string), age (int), is_student (bool)");
            }
            Person person;
            person.name = obj["name"].asString();
            person.age = obj["age"].asInt();
            person.is_student = obj["is_student"].asBool();
            return Json::Value(process_person_impl(person));
        });

        server.register_function("get_greetings", [](const Json::Value& args) -> Json::Value {
            // TODO: Implement wrapper for get_greetings_impl
            // 1. Validate args: should be a JSON array containing one element, which is an array of strings.
            // 2. Extract names from the inner JSON array into a std::vector<std::string>.
            // 3. Call get_greetings_impl with the vector.
            // 4. Convert the resulting std::vector<std::string> back to a JSON array of strings.
             Json::Value result_json(Json::arrayValue); // Create JSON array
            if (!args.isArray() || args.size() != 1 || !args[0].isArray()) {
                throw std::invalid_argument("get_greetings requires one array of strings");
            }
            std::vector<std::string> names;
            for (const auto& item : args[0]) {
                if (!item.isString()) {
                    throw std::invalid_argument("All elements must be strings");
                }
                names.push_back(item.asString());
            }
            auto greetings = get_greetings_impl(names);
            
            Json::Value result_json(Json::arrayValue);
            for (const auto& greeting : greetings) {
                result_json.append(greeting);
            }
            return result_json;
        });

        server.start(); // This will block until the server is stopped

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server exiting." << std::endl;
    return 0;
}
