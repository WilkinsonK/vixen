#include <sstream>

#include "include/vixen/symbols.hpp"
#include "tests/vixen/test_testconf.hpp"

namespace test_vixen::symbols {
    using namespace std;
    using namespace vixen::symbols;

    void test_string_contains_char() {
        assert(char_in_string('c', "charlie") == 1, "'c' is present but not found in 'charlie'.");
        assert(char_in_string('*', "starlord") != 1, "'*' was found in 'starlord'.");
    }

    void test_char_iscomment() {
        assert(char_iscomment('#') == 1, "'#' should be valid comment character.");
        assert(char_iscomment('@') == 0, "'@' should not be valid comment character.");
    }

    void test_char_isdigitchar() {
        for (auto const& ch : "1234567890") {
            if (ch == '\0')
                continue;
            assert(char_isdigitchar(ch) == 1, "'{}' should be a valid digit character.", ch);
        }
    }

    void test_char_isdigittext() {
        std::string chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

        for (auto const& ch : chars) {
            if (ch == '\0')
                continue;
            assert(char_isdigitext(ch) == 1, "'{}' should be valid digit extension char.", ch);
        }
    }

    void test_char_isdigitsep() {
        for (auto const& ch : "-_.bdxo") {
            if (ch == '\0')
                continue;
            assert(char_isdigitsep(ch) == 1, "'{}' should be a valid digit separation char.", ch);
        }
    }

    void test_char_isgroupchar() {
        for (auto const& ch : ")}][{(") {
            if (ch == '\0')
                continue;
            assert(char_isgroupchar(ch) == 1, "'{}' should be a valid grouping character.", ch);
        }
    }

    void test_char_isnamechar() {
        std::string valid_chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "_");

        std::string invalid_chars("!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~");

        for (auto const& ch : valid_chars) {
            if (ch == '\0')
                continue;
            assert(char_isnamechar(ch) == 1, "'{}' should be a valid name character.", ch);
        }

        for (auto const& ch : invalid_chars) {
            if (ch == '\0')
                continue;
            assert(char_isnamechar(ch) == 0, "'{}' should not be a valid name character.", ch);
        }
    }

    void test_char_isnewline() {
        assert(char_isnewline('\n') == 1, "'\\n' should be a valid newline character.");
    }
}
