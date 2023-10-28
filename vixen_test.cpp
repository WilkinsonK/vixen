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
    attempt("string_contains_char", test_vixen::symbols::test_string_contains_char);
    attempt("char_iscommment", test_vixen::symbols::test_char_iscomment);
    attempt("char_isdigitchar", test_vixen::symbols::test_char_isdigitchar);
    attempt("char_isdigittext", test_vixen::symbols::test_char_isdigittext);
    attempt("char_isdigitsep", test_vixen::symbols::test_char_isdigitsep);

    dump_results();
}
