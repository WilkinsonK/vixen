#include "tests/vixen/test_testconf.hpp"
#include "tests/test_vixen.hpp"

using namespace test_vixen;

int main(void) {
    assert(1 != 2, "1 must not equal 2.");

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
    add_test("symbols::string_has_char", test_vixen::symbols::test_string_contains_char);
    add_test("symbols::char_iscommment", test_vixen::symbols::test_char_iscomment);
    add_test("symbols::char_isdigitchar", test_vixen::symbols::test_char_isdigitchar);
    add_test("symbols::char_isdigittext", test_vixen::symbols::test_char_isdigittext);
    add_test("symbols::char_isdigitsep", test_vixen::symbols::test_char_isdigitsep);
    add_test("symbols::char_isgroupchar", test_vixen::symbols::test_char_isgroupchar);
    add_test("symbols::char_isnamechar", test_vixen::symbols::test_char_isnamechar);
    add_test("symbols::char_isnewline", test_vixen::symbols::test_char_isnewline);
    add_test("symbols::char_isnoparse", test_vixen::symbols::test_char_isnoparse);
    add_test("symbols::char_ispuncchar", test_vixen::symbols::test_char_ispuncchar);
    add_test("symbols::char_isstrchar", test_vixen::symbols::test_char_isstrchar);
    add_test("symbols::char_istermchar", test_vixen::symbols::test_char_istermchar);
    add_test("symbols::symbol_isname", test_vixen::symbols::test_symbol_isname);
    add_test("symbols::symbol_isnumeric", test_vixen::symbols::test_symbol_isnumeric);
    add_test("symbols::symbol_ispunc", test_vixen::symbols::test_symbol_ispunc);
    add_test("symbols::symbol_isstrsym", test_vixen::symbols::test_symbol_isstrsym);
    add_test("symbols::symbol_istermed", test_vixen::symbols::test_symbol_istermed);
    add_test("symbols::symbol_next_isvalidname", test_vixen::symbols::test_symbol_next_isvalidname);
    add_test("symbols::symbol_next_isvalidnum", test_vixen::symbols::test_symbol_next_isvalidnum);
    add_test("symbols::parser_no_error", test_vixen::symbols::test_parser_no_error);
    add_test("symbols::parser_no_whitespace", test_vixen::symbols::test_parser_nowhitespace);

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

    dump_header();
    attempt_all();
    dump_results();
}
