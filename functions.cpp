#include "functions.h"
#include <iostream>
#include <stdexcept>
#include <string> // Include string header
#include <vector> // Include vector header
#include <numeric> // For std::accumulate

// --- Basic Data Type Function Implementations ---

int32_t add_impl(int32_t a, int32_t b) {
    return a + b;
}

std::string greet_impl(const std::string& name) {
    return std::string("Hello, ") + name + "!";
}

bool is_positive_impl(float num) {
    return num > 0;
}

// echo is handled directly in the main.cpp wrapper

void no_return_impl() {
    // Function that does something but returns nothing
    std::cout << "Executing no_return_impl()..." << std::endl;
}

int32_t divide_impl(int32_t numerator, int32_t denominator) {
    if (denominator == 0) {
        throw std::runtime_error("Division by zero");
    }
    return numerator / denominator;
}

// --- Complex Data Type Function Implementations ---

int32_t sum_array_impl(const std::vector<int32_t>& numbers) {
    // Use std::accumulate to sum the elements
    return std::accumulate(numbers.begin(), numbers.end(), 0);
}

std::string process_person_impl(const Person& person_data) {
    std::string status = person_data.is_student ? "a student" : "not a student";
    return "Processed person: " + person_data.name + ", age " + std::to_string(person_data.age) + ", is " + status + ".";
}

std::vector<std::string> get_greetings_impl(const std::vector<std::string>& names) {
    std::vector<std::string> greetings;
    greetings.reserve(names.size()); // Optimize allocation
    for (const auto& name : names) {
        greetings.push_back(greet_impl(name)); // Reuse existing greet function
    }
    return greetings;
}
