// Houndog
// ------------------------------------------------------------------
// Vixen Programming Language testing framework.
// Author: Keenan W. Wilkinsonk
// Date: 31 Oct 2023
// ------------------------------------------------------------------
// Adapted from original test_testconf.hpp, this header file defines
// tools Vixen uses to test its internal API.
#pragma once
#include <format>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <sys/ioctl.h>
#include <unistd.h>

#include "ggm.hpp"

namespace hounddog {
    typedef void(*TestCaseFunc)(void);

    // Test runtime statistics.
    struct TestRunStats {
        std::unordered_map<std::string, TestCaseFunc> registry;
        uint tests_attempted = 0;
        uint tests_succeeded = 0;
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

    void add_test(
        TestRunStats& trs,
        const std::string& id,
        TestCaseFunc tc);
    template <class... Args>
    void assert(const bool result, const std::string& reason, Args&&... args);
    void assert_error(void(*wrapped)(), const std::string& reason);
    void assert_noerr(void(*wrapped)(), const std::string& reason);
    void attempt(TestRunStats& trs, const std::string& pattern, std::ostream& os);
    void attempt_all(TestRunStats& trs, std::ostream& os);
    void attempt_one(TestRunStats& trs, const std::string& id, std::ostream& os);
    template<class... Args>
    void dump_header(
        TestRunStats& trs,
        const std::string& title,
        std::ostream& os,
        Args&&... args);
    void dump_result(TestRunStats& trs, std::ostream& os);
    void dump_title(TestRunStats& trs, std::ostream& os);

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

    // Attempt registered test cases whos id
    // matches the pattern.
    void attempt(TestRunStats& trs, const std::string& pattern, std::ostream& os) {
        std::vector<std::string> ids;

        // Collect matching ids.
        for (auto const& [id, _] : trs.registry) {
            if (ggm::gitignore_glob_match(id.c_str(), pattern.c_str()))
                ids.push_back(id);
        }

        if (!ids.size()) {
            std::cerr
                << "error: no test case ids match pattern '"
                << pattern
                << "'."
                << std::endl;
            exit(1);
        }

        dump_title(trs, os);
        os
        << "TESTING: '"
        << pattern
        << "' (" << ids.size() << "/" << trs.registry.size() << " available)"
        << std::endl;

        for (auto const& id : ids)
            attempt_one(trs, id, os);
        dump_result(trs, os);
    }

    // Attempt all registered test cases.
    void attempt_all(TestRunStats& trs, std::ostream& os) {
        dump_title(trs, os);
        os << "TESTING: all" << std::endl;
        for (auto const& [id, _] : trs.registry)
            attempt_one(trs, id, os);
        dump_result(trs, os);
    }

    // Try running the test case associated with
    // the ID. Writes to output stream if the test
    // case fails.
    void attempt_one(TestRunStats& trs, const std::string& id, std::ostream& os) {
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

        // Gutters.
        const std::string gutter(std::min((int)ws.ws_col, 78), '-');

        os
        << gutter << " |\n"
        << std::vformat(title, std::make_format_args(args...)) << "\n"
        << gutter << " |"
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
        << "TOTAL:   " << trs.tests_attempted
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
