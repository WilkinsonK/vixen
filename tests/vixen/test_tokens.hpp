#include <fstream>

#include "include/vixen/tokens.hpp"
#include "tests/hounddog.hpp"

namespace test_vixen::tokens {
    using namespace std;
    using namespace vixen::tokens;

    Lexer setup_lexer() {
        const std::string file_name("examples/test_symbols.vxn");
        ifstream file(file_name);
        hounddog::assert(file.is_open(), "Could not open test file '{}'", file_name);

        Lexer lexer(file, file_name);
        file.close();
        return lexer;
    }

    Token setup_token() {
        return Token(20, 6, "this_is_a_token");
    }

    void test_find_errunk() {
        uint tt = (uint)tokens_find_errunk("dummy_symbol");
        hounddog::assert(tt == 1, "Must return 'TokenType::ErrorUnknown(1) not'{}'", tt);
    }
}