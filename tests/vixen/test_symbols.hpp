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
        std::string invalid_chars("!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~");
        std::string valid_chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "_");


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

    void test_char_isnoparse() {
        for (auto const& ch : " \t\n\r\v\f") {
            if (ch == '\0')
                continue;
            assert(char_isnoparse(ch) == 1, "'{}' should be valid whitespace.", ch);
        }
    }

    void test_char_ispuncchar() {
        std::string valid_chars("!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~");
        std::string invalid_chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "_");

        for (auto const& ch : valid_chars) {
            if (ch == '\0')
                continue;
            assert(char_ispuncchar(ch) == 1, "'{}' should be a valid punctuation character.", ch);
        }

        for (auto const& ch : invalid_chars) {
            if (ch == '\0')
                continue;
            assert(char_ispuncchar(ch) == 0, "'{}' should not be a valid punctuation character.", ch);
        }
    }

    void test_char_isstrchar() {
        for (auto const& ch : "'`\"") {
            if (ch == '\0')
                continue;
            assert(char_isstrchar(ch) == 1, "'{}' should be a valid string annotation.", ch);
        }
    }

    void test_char_istermchar() {
        assert(char_istermchar(';') == 1, "';' should be a valid line terminating character.");
    }

    void test_symbol_isname() {
        assert(symbol_isname("fibonacci") == 1, "'fibonacci' should be a valid name symbol.");
        assert(symbol_isname("f&bonacci") == 0, "'f&bonacci' should not be a valid name symbol; names must not contain punctuation.");
        assert(symbol_isname("0ibonacci") == 0, "'0ibonacci' should not be a valid name symbol; names must not start with digits.");
    }

    void test_symbol_isnumeric() {
        std::string valid_symbols[] = {
            ".4",
            "0x5",
            "0o77",
            "0b1011",
            "3.14",
            "-1",
            "339",
            "100_000"
        };

        for (auto const& symbol : valid_symbols)
            assert(symbol_isnumeric(symbol) == 1, "'{}' should be a valid numberical symbol.", symbol);
    }

    void test_symbol_ispunc() {
        assert(symbol_ispunc("") == 0, "Empty string is not a valid symbol.");
        assert(symbol_ispunc("fibonacci") == 0, "'fibonacci' should not be a valid punctuation symbol.");
        assert(symbol_ispunc("0\%alpha") == 0, "Punctuation cannot contain alpha numeric characters.");
        assert(symbol_ispunc("%&") == 1, "'%&' should be a valid punctuation symbol.");
    }

    void test_symbol_isstrsym() {
        std::string valid_stringsyms[] = {
            "'", "'''", "`", "```", "\"", "\"\"\""};

        for (auto const& symbol : valid_stringsyms)
            assert(symbol_isstrsym(symbol) == 1, "'{}' should be a valid string symbol.", symbol);
    }

    void test_symbol_istermed() {
        assert(symbol_istermed("na;me", ';') == 0, "'na;me' should not be valid terminated symbol; termination character in symbol.");
        assert(symbol_istermed("nam", 'e') == 0, "Next character 'e' should not be valid termination character.");
        assert(symbol_istermed("name", ';') == 1, "'name' is terminated by next character ';'.");
    }

    void test_symbol_next_isvalidname() {
        assert(symbol_next_isvalidname("gregory", '+') == 0, "'+' is not compatible for name symbol.");
        assert(symbol_next_isvalidname("nam", 'e') == 1, "'nam' with 'e' should compose a valid name.");
    }

    void test_symbol_next_isvalidnum() {
        assert(symbol_next_isvalidnum(".", '0') == 1, "'.0' should create a valid numerical symbol.");
        assert(symbol_next_isvalidnum("0x", 'f') == 1, "Hexidecimal numericals should allow for alpha characters.");
        assert(symbol_next_isvalidnum("-", '1') == 1, "'-' should be valid when the next character is numeric.");
        assert(symbol_next_isvalidnum("12_", '7') == 1, "'12_' and '7' should compose a valid numerical symbol.");
    }
}
