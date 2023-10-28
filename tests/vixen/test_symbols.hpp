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
        for (auto const& ch : "123a4567890") {
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
}