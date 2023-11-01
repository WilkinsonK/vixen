#include "tests/hounddog.hpp"
#include "tests/vixen.hpp"

int main(int argc, const char* argv[]) {
    hounddog::TestRunStats trs;

    hounddog::assert(1 != 2, "1 must not equal 2.");

    // Vixen Front End Interface Components.
    // ------------------------------------------
    // This section focuses on the parts of our
    // language that digests raw information from
    // user input into a structure our backend
    // can eventually turn into something that's
    // machine readable.

    // Vixen symbol Parsing Suite.
    // ------------------------------------------
    // These functions perform tests that define
    // symbol parsing behavior. That is, we use
    // the below to make our expected behavior
    // more concrete.
    // NOTICE: These tests are critical to the
    // foundation of our front-end. Without them,
    // our language might as well not work at all.
    hounddog::add_test(trs, "symbols::string_has_char", test_vixen::symbols::test_string_contains_char);
    hounddog::add_test(trs, "symbols::char_iscommment", test_vixen::symbols::test_char_iscomment);
    hounddog::add_test(trs, "symbols::char_isdigitchar", test_vixen::symbols::test_char_isdigitchar);
    hounddog::add_test(trs, "symbols::char_isdigittext", test_vixen::symbols::test_char_isdigittext);
    hounddog::add_test(trs, "symbols::char_isdigitsep", test_vixen::symbols::test_char_isdigitsep);
    hounddog::add_test(trs, "symbols::char_isgroupchar", test_vixen::symbols::test_char_isgroupchar);
    hounddog::add_test(trs, "symbols::char_isnamechar", test_vixen::symbols::test_char_isnamechar);
    hounddog::add_test(trs, "symbols::char_isnewline", test_vixen::symbols::test_char_isnewline);
    hounddog::add_test(trs, "symbols::char_isnoparse", test_vixen::symbols::test_char_isnoparse);
    hounddog::add_test(trs, "symbols::char_ispuncchar", test_vixen::symbols::test_char_ispuncchar);
    hounddog::add_test(trs, "symbols::char_isstrchar", test_vixen::symbols::test_char_isstrchar);
    hounddog::add_test(trs, "symbols::char_istermchar", test_vixen::symbols::test_char_istermchar);
    hounddog::add_test(trs, "symbols::symbol_isname", test_vixen::symbols::test_symbol_isname);
    hounddog::add_test(trs, "symbols::symbol_isnumeric", test_vixen::symbols::test_symbol_isnumeric);
    hounddog::add_test(trs, "symbols::symbol_ispunc", test_vixen::symbols::test_symbol_ispunc);
    hounddog::add_test(trs, "symbols::symbol_isstrsym", test_vixen::symbols::test_symbol_isstrsym);
    hounddog::add_test(trs, "symbols::symbol_istermed", test_vixen::symbols::test_symbol_istermed);
    hounddog::add_test(trs, "symbols::symbol_next_isvalidname", test_vixen::symbols::test_symbol_next_isvalidname);
    hounddog::add_test(trs, "symbols::symbol_next_isvalidnum", test_vixen::symbols::test_symbol_next_isvalidnum);
    hounddog::add_test(trs, "symbols::parser_no_error", test_vixen::symbols::test_parser_no_error);
    hounddog::add_test(trs, "symbols::parser_no_whitespace", test_vixen::symbols::test_parser_nowhitespace);
    hounddog::add_test(trs, "symbols::parser_parse_expected", test_vixen::symbols::test_parser_parse_expected);

    // Vixen Token Parsing Suite.
    // ------------------------------------------
    // TLDR; this suite is basically just an
    // extension of the symbols suite.
    // `tokens.hpp` is built off of our symbols
    // suite as a more specialized parser where
    // symbols are categorized as `Token` objects.
    // The `Token`s are used as classifiers to
    // assist in organizing our symbols at parse
    // time.

    // Current driver code.
    switch (argc) {
        case 1:
            hounddog::attempt_all(trs, std::cout);
            break;
        case 2:
            hounddog::attempt(trs, argv[1], std::cout);
            break;
        default:
            std::cerr
                << "error: "
                << argv[0] << " accepts 1 or 0 arguments (pattern?)"
                << std::endl;
            return 1;
    }

    return 0;
}
