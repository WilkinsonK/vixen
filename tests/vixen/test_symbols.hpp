#include <sstream>

#include "tests/hounddog.hpp"
#include "include/vixen/symbols.hpp"

namespace test_vixen::symbols {
    using namespace vixen::symbols;

    RawParser setup_parser() {
        const std::string file_name("examples/test_symbols.vxn");
        ifstream file(file_name);
        hounddog::assert(file.is_open(), "Could not open test file '{}'", file_name);

        RawParser parser(file, file_name);
        file.close();
        return parser;
    }

    void test_string_contains_char() {
        hounddog::assert(!char_in_string('*', "starlord"), "'*' was found in 'starlord'.");
        hounddog::assert(char_in_string('c', "charlie"), "'c' is present but not found in 'charlie'.");
    }

    void test_char_iscomment() {
        hounddog::assert(!char_iscomment('@'), "'@' should not be valid comment character.");
        hounddog::assert(char_iscomment('#'), "'#' should be valid comment character.");
    }

    void test_char_isdigitchar() {
        for (auto const& ch : "1234567890") {
            if (ch == '\0')
                continue;
            hounddog::assert(char_isdigitchar(ch), "'{}' should be a valid digit character.", ch);
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
            hounddog::assert(char_isdigitext(ch), "'{}' should be valid digit extension char.", ch);
        }
    }

    void test_char_isdigitsep() {
        for (auto const& ch : "-_.bdxo") {
            if (ch == '\0')
                continue;
            hounddog::assert(char_isdigitsep(ch), "'{}' should be a valid digit separation char.", ch);
        }
    }

    void test_char_isgroupchar() {
        for (auto const& ch : ")}][{(") {
            if (ch == '\0')
                continue;
            hounddog::assert(char_isgroupchar(ch), "'{}' should be a valid grouping character.", ch);
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
            hounddog::assert(char_isnamechar(ch), "'{}' should be a valid name character.", ch);
        }

        for (auto const& ch : invalid_chars) {
            if (ch == '\0')
                continue;
            hounddog::assert(!char_isnamechar(ch), "'{}' should not be a valid name character.", ch);
        }
    }

    void test_char_isnewline() {
        hounddog::assert(char_isnewline('\n'), "'\\n' should be a valid newline character.");
    }

    void test_char_isnoparse() {
        for (auto const& ch : " \t\n\r\v\f") {
            if (ch == '\0')
                continue;
            hounddog::assert(char_isnoparse(ch), "'{}' should be valid whitespace.", ch);
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
            hounddog::assert(char_ispuncchar(ch), "'{}' should be a valid punctuation character.", ch);
        }

        for (auto const& ch : invalid_chars) {
            if (ch == '\0')
                continue;
            hounddog::assert(!char_ispuncchar(ch), "'{}' should not be a valid punctuation character.", ch);
        }
    }

    void test_char_isstrchar() {
        for (auto const& ch : "'`\"") {
            if (ch == '\0')
                continue;
            hounddog::assert(char_isstrchar(ch), "'{}' should be a valid string annotation.", ch);
        }
    }

    void test_char_istermchar() {
        hounddog::assert(char_istermchar(';'), "';' should be a valid line terminating character.");
    }

    void test_symbol_isname() {
        hounddog::assert(!symbol_isname("f&bonacci"), "'f&bonacci' should not be a valid name symbol; names must not contain punctuation.");
        hounddog::assert(!symbol_isname("0ibonacci"), "'0ibonacci' should not be a valid name symbol; names must not start with digits.");
        hounddog::assert(symbol_isname("fibonacci"), "'fibonacci' should be a valid name symbol.");
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
            hounddog::assert(symbol_isnumeric(symbol), "'{}' should be a valid numberical symbol.", symbol);
    }

    void test_symbol_ispunc() {
        hounddog::assert(!symbol_ispunc(""), "Empty string is not a valid symbol.");
        hounddog::assert(!symbol_ispunc("fibonacci"), "'fibonacci' should not be a valid punctuation symbol.");
        hounddog::assert(!symbol_ispunc("0\%alpha"), "Punctuation cannot contain alpha numeric characters.");
        hounddog::assert(symbol_ispunc("%&"), "'%&' should be a valid punctuation symbol.");
    }

    void test_symbol_isstrsym() {
        std::string valid_stringsyms[] = {
            "'", "'''", "`", "```", "\"", "\"\"\""};

        for (auto const& symbol : valid_stringsyms)
            hounddog::assert(symbol_isstrsym(symbol), "'{}' should be a valid string symbol.", symbol);
    }

    void test_symbol_istermed() {
        hounddog::assert(!symbol_istermed("na;me", ';'), "'na;me' should not be valid terminated symbol; termination character in symbol.");
        hounddog::assert(!symbol_istermed("nam", 'e'), "Next character 'e' should not be valid termination character.");
        hounddog::assert(symbol_istermed("name", ';'), "'name' is terminated by next character ';'.");
    }

    void test_symbol_next_isvalidname() {
        hounddog::assert(!symbol_next_isvalidname("gregory", '+'), "'+' is not compatible for name symbol.");
        hounddog::assert(symbol_next_isvalidname("nam", 'e'), "'nam' with 'e' should compose a valid name.");
    }

    void test_symbol_next_isvalidnum() {
        hounddog::assert(symbol_next_isvalidnum(".", '0'), "'.0' should create a valid numerical symbol.");
        hounddog::assert(symbol_next_isvalidnum("0x", 'f'), "Hexidecimal numericals should allow for alpha characters.");
        hounddog::assert(symbol_next_isvalidnum("-", '1'), "'-' should be valid when the next character is numeric.");
        hounddog::assert(symbol_next_isvalidnum("12_", '7'), "'12_' and '7' should compose a valid numerical symbol.");
    }

    void test_parser_no_error() {
        hounddog::assert_noerr(
            [](){setup_parser();},
            "Requesting parser should not throw system error.");
        hounddog::assert_noerr(
            [](){
                RawParser p = setup_parser();
                while (!p.end()) p.next();
            },
            "Symbol parsing from input should not throw a system error."
        );
    }

    void test_parser_nowhitespace() {
        RawParser p = setup_parser();

        // Validates that an individual symbol,
        // that is not a string symbol, does not
        // contain any whitespace characters.
        auto assert_nowhitespace = [](const Symbol& symbol) {
            for (auto const& ch : " \t\n\r\v\f") {
                if (ch == '\0')
                    continue;
                hounddog::assert(!(char_in_string(ch, symbol)), "'{}' should not exist in non-string symbol '{}'.", ch, symbol);
            }
        };

        while (!p.end()) {
            auto const [lineno, column, symbol] = p.next();
            if (!p.string_mode()) {
                assert_nowhitespace(symbol);
            }
        }
    }

    void test_parser_parse_expected() {
        RawParser p = setup_parser();
        TRIPLET(Symbol) symbols_expected[] = {
            {5, 1, "sx"},
            {5, 3, ":"},
            {5, 5, "int"},
            {5, 9, "="},
            {5, 11, "0"},
            {5, 12, ";"},
            {6, 1, "cx"},
            {6, 3, ":"},
            {6, 5, "str"},
            {6, 9, "="},
            {6, 11, "\'\'\'"},
            {6, 14, "d%\'-\'`"},
            {6, 20, "\'\'\'"},
            {6, 23, ";"},
            {7, 1, "kv"},
            {7, 3, ":"},
            {7, 5, "str"},
            {7, 9, "="},
            {7, 11, "\""},
            {7, 12, "{interpol} this"},
            {7, 27, "\""},
            {7, 28, ";"},
            {8, 1, "x"},
            {8, 2, "++"},
            {8, 4, ";"},
            {8, 6, "s"},
            {8, 7, ":"},
            {8, 9, "flt"},
            {8, 13, "="},
            {8, 15, "49.9"},
            {8, 19, ".3"},
            {8, 21, ";"},
            {11, 1, "EOF"},
        };

        for (auto const& expected : symbols_expected) {
            auto const& parsed = p.next();
            auto const& [exlineno, excolumn, exsymbol] = expected;
            auto const& [prlineno, prcolumn, prsymbol] = parsed;

            hounddog::assert(
                expected == parsed,
                "Expected '{}' @({}, {}). Got '{}' @({}, {})",
                exsymbol,
                exlineno,
                excolumn,
                prsymbol,
                prlineno,
                prcolumn);
        }
    }
}
