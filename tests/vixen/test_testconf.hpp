#pragma once
#include <format>
#include <iostream>
#include <sstream>

typedef void(*TestCaseFunc)(void);

static uint test_cases_attempted = 0;
static uint test_cases_succeeded = 0;

// If some assertion fails, this error is raised
// with the provided error message from the user.
class TestAssertionError : public std::exception {
    private:
        std::string reason;

    public:
        template <typename... Args>
        TestAssertionError(const std::string& reason, Args&&... args) {
            this->reason = std::vformat(reason, std::make_format_args(args...));
        }

        char* what() {
            return (char*)(this->reason.c_str());
        }
};

// Using custom assert for string formatting.
// Tests if the given statement is true or
// false. If false, writes to stderr and then
// exits.
template <class... Args>
void assert(const int res, const std::string& reason, Args&&... args) {
    if (res) return;
    throw TestAssertionError(reason, args...);
}

// Try running the test case. Writes to stderr if
// the test case fails.
void attempt(const std::string& name, TestCaseFunc tc) {
    std::cout << "attempting test \"" << name << "\": ";
    test_cases_attempted++;

    try {
        tc();
    } catch (TestAssertionError err) {
        std::cout << "failure(" << err.what() << ")" << std::endl;
        return;
    }

    std::cout << "success" << std::endl;
    test_cases_succeeded++;
}

// Dump the test results to stdout.
void dump_results() {
    std::cout
        << "----------------------------------------------------------------\n"
        << "Vixen Test Results\n"
        << "----------------------------------------------------------------\n"
        << "SUCCESS: " << test_cases_succeeded << "\n"
        << "FAILURE: " << test_cases_attempted - test_cases_succeeded << "\n"
        << "TOTAL:   " << test_cases_attempted
        << std::endl;
}
