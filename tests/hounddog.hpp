// Houndog
// Vixen Programming Language testing framework.
// Author: Keenan W. Wilkinsonk
// Date: 31 Oct 2023
// Adapted from original test_testconf.hpp, this header file defines
// tools Vixen uses to test its internal API.
#pragma once
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <sys/ioctl.h>
#include <unistd.h>

namespace hounddog {
    typedef void(*TestCaseFunc)(void);

    // Test runtime statistics.
    struct TestRunStats {
        std::unordered_map<std::string, TestCaseFunc> registry;
        uint tests_attempted;
        uint tests_succeeded;
    };

    // If some assertion failes, this error is
    // raised with the provided error message from
    // the user.
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

    // Add a test to the registry.
    void add_test(
        TestRunStats& trs,
        const std::string& id,
        TestCaseFunc tc) {

        trs.registry[id] = tc;
    }

    // Using custom assert for string formatting.
    // Tests if the given statement is true or
    // false. If false, writes to stderr and then
    // exits.
    template <class... Args>
    void assert(const bool result, const std::string& reason, Args&&... args) {
        if (result) return;
        throw TestAssertionError(reason, args...);
    }

    // A specialized assertion. This function
    // accepts a function pointer to be wrapped in
    // a generic try-catch block to throw an
    // assertion error if the wrapped does not
    // throw a system error.
    void assert_error(void(*wrapped)(), const std::string& reason) {
        try {
            wrapped();
        } catch (...) {
            return;
        }
        assert(false, "assert_error failed with message '{}'", reason);
    }

    // A specialized assertion. This function
    // accepts a function pointer to be wrapped
    // in a generic try-catch block to throw an
    // assertion error if the wrapped function
    // throws some system error.
    void assert_noerr(void(*wrapped)(), const std::string& reason) {
        try {
            wrapped();
        } catch (const std::exception& err) {
            assert(
                false,
                "assert_noerr failed with message '{}'. Got '{}'",
                reason,
                err.what());
        }
    }

    // Try running the test case associated with
    // the ID. Writes to output stream if the test
    // case fails.
    void attempt(TestRunStats& trs, const std::string& id, std::ostream& os) {
        os << "attempting test \"" << id << "\": ";
        trs.tests_attempted++;

        try {
            trs.registry[id]();
            trs.tests_succeeded++;
            os << "success";
        } catch (TestAssertionError& err) {
            os << "failure(" << err.what() << ")";
        } catch (const std::exception& err) {
            os
            << "failure(unexpected exception)\n"
            << "\terror: " << err.what() << "\n";
        }
        os << std::endl;
    }

    // Attempt all registered test cases.
    void attempt_all(TestRunStats& trs, std::ostream& os) {
        for (auto const& [id, _] : trs.registry) {
            attempt(trs, id, os);
        }
    }

    // Dump the header to string to an output
    // stream.
    template <class... Args>
    void dump_header(
        TestRunStats& trs,
        const std::string& title,
        std::ostream& os,
        Args&&... args) {

        // Get terminal dimensions.
        struct winsize ws;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

        os
        << std::string(ws.ws_col, '-') << "\n"
        << std::vformat(title, std::make_format_args(args...)) << "\n"
        << std::string(ws.ws_col, '-')
        << std::endl;
    }

    // Dump the test results to an output stream.
    void dump_result(TestRunStats& trs, std::ostream& os) {
        dump_header(trs, "Houndog Test Results", os);
        os << "SUCCESS: " << trs.tests_succeeded << "\n";

        uint tests_failed = (trs.tests_attempted - trs.tests_succeeded);
        double success_percentage =
            ((double)trs.tests_succeeded / (double)trs.tests_attempted)*100;

        if (tests_failed)
            os << "FAILURE: " << tests_failed << "\n";

        os
        << "TOTAL: " << trs.tests_attempted
        << " (" << std::setprecision(4) << success_percentage << "\% passing)"
        << std::endl;
    }

    // Dump the test title to an output stream.
    void dump_title(TestRunStats& trs, std::ostream& os) {
        dump_header(
            trs,
            "Houndog Test Suite | REGISTERED: {}",
            os,
            trs.registry.size());
    }
}
