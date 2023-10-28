#pragma once
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

typedef void(*TestCaseFunc)(void);

static std::unordered_map<std::string, TestCaseFunc> TestCaseRegistry = {};
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

// Register a test case to be used later.
void add_test(const std::string& name, TestCaseFunc tc) {
    TestCaseRegistry[name] = tc;
}

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
void attempt(const std::string& name) {
    TestCaseFunc tc = TestCaseRegistry[name];
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

// Attempt all registered test cases.
void attempt_all() {
    for (auto const& [name, tc] : TestCaseRegistry) {
        attempt(name);
    }
}

// Dump test header to stdout.
void dump_header() {
    std::cout
        << "----------------------------------------------------------------\n"
        << "Vixen Test Suite | REGISTERED: " << TestCaseRegistry.size() << "\n"
        << "----------------------------------------------------------------"
        << std::endl;
}

// Dump the test results to stdout.
void dump_results() {
    uint test_cases_failed = test_cases_attempted - test_cases_succeeded;
    double success_percentage =
        ((double)test_cases_succeeded /
        (double)test_cases_attempted)*100;

    std::cout
        << "----------------------------------------------------------------\n"
        << "Vixen Test Results\n"
        << "----------------------------------------------------------------\n"
        << "SUCCESS: " << test_cases_succeeded << "\n";

    if (test_cases_failed)
        std::cout << "FAILURE: " << test_cases_failed << "\n";

    std::cout
        << "TOTAL:   " << test_cases_attempted
        << " (" << std::setprecision(4) << success_percentage << "\% passing)"
        << std::endl;
}
