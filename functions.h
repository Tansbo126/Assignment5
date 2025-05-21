#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <cstdint> // For int32_t

// --- Basic Data Type Functions ---
// Function prototypes using basic C++ types
// These are the functions that perform the actual logic.
// The wrappers in main.cpp will handle JSON conversion.

int32_t add_impl(int32_t a, int32_t b);
std::string greet_impl(const std::string& name);
bool is_positive_impl(float num);
// For echo, the wrapper in main.cpp can handle returning the Json::Value directly
// For no_return, the wrapper handles returning Json::null
void no_return_impl();
int32_t divide_impl(int32_t numerator, int32_t denominator); // Can throw std::runtime_error


// --- Complex Data Type Functions---

struct Person {
    std::string name;
    int32_t age;
    bool is_student;
};

// Function prototypes
int32_t sum_array_impl(const std::vector<int32_t>& numbers);
std::string process_person_impl(const Person& person_data);
std::vector<std::string> get_greetings_impl(const std::vector<std::string>& names);


#endif // FUNCTIONS_H
