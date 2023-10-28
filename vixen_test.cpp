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

    dump_header();
    attempt_all();
    dump_results();
}
