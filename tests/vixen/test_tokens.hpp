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

    void test_find_errunk() {
        uint tt = (uint)tokens_find_errunk("dummy_symbol");
        hounddog::assert(tt == 1, "Must return 'TokenType::ErrorUnknown(1) not '{}'", tt);
    }

    void test_find_genname() {
        hounddog::assert(tokens_find_genname("dummy_symbol") == "NameGeneric", "A non-specific symbol should return 'NameGeneric' as the token type name");
        hounddog::assert(tokens_find_genname("==") == "OperEquals", "'==' shoule return 'OperEquals' as the token type name");
        hounddog::assert(tokens_find_genname(TokenType::KwdDelete) == "KwdDelete", "'TokenType::KwdDelete' should return 'KwdDelete' as the token type name");
    }

    void test_find_gentype() {
        uint tt;
        tt = (uint)tokens_find_gentype("EOF");
        hounddog::assert(tt == 87, "Must return 'TokenType::CTRLCharEOF(87)' not '{}'", tt);

        tt = (uint)tokens_find_gentype("dummy_symbol");
        hounddog::assert(tt == 31, "Must return 'TokenType::NameGeneric(31)' not '{}'", tt);

        tt = (uint)tokens_find_gentype("->");
        hounddog::assert(tt == 64, "Must return 'TokenType::OperAttrPtr(64)' not '{}'", tt);
    }

    void test_find_numtype() {
        uint tt;

        tt = (uint)tokens_find_numtype("0b010101111");
        hounddog::assert(tt == 33, "Must return 'TokenType::NumBin(33)' not '{}'", tt);

        tt = (uint)tokens_find_numtype("0x0af");
        hounddog::assert(tt == 35, "Must return 'TokenType::NumHex(35)' not '{}'", tt);

        tt = (uint)tokens_find_numtype("175.0");
        hounddog::assert(tt == 34, "Must return 'TokenType::NumFlt(34)' not '{}'", tt);

        tt = (uint)tokens_find_numtype("0o257");
        hounddog::assert(tt == 37, "Must return 'TokenType::NumOct(37)' not '{}'", tt);

        tt = (uint)tokens_find_numtype("175");
        hounddog::assert(tt == 36, "Must return 'TokenType::NumInt(36)' not '{}'", tt);
    }

    void test_isfloat() {
        Lexer l = setup_lexer();
        Token t;

        // Skip to next floating point token.
        for (int i = 0; i < 30; i++)
            l.next();
        t = l.next();
        hounddog::assert(tokens_isfloat(t), "{} should be a valid float token", t.symbol);
        hounddog::assert(!tokens_isinteger(t), "{} should not be a valid integer token", t.symbol);
        hounddog::assert(!tokens_isgeneric(t), "{} should not be a valid name token", t.symbol);
    }

    void test_isgeneric() {
        Lexer l = setup_lexer();
        Token t;

        t = l.next();
        hounddog::assert(!tokens_isfloat(t), "{} should not be a valid float token", t.symbol);
        hounddog::assert(!tokens_isinteger(t), "{} should not be a valid integer token", t.symbol);
        hounddog::assert(tokens_isgeneric(t), "{} should be a valid name token", t.symbol);
    }

    void test_isinteger() {
        Lexer l = setup_lexer();
        Token t;

        // Skip to next integer token.
        for (int i = 0; i < 4; i++)
            l.next();
        t = l.next();
        hounddog::assert(!tokens_isfloat(t), "'{}' should not be a valid float token", t.symbol);
        hounddog::assert(tokens_isinteger(t), "'{}' should be a valid integer token", t.symbol);
        hounddog::assert(!tokens_isgeneric(t), "'{}' should not be a valid name token", t.symbol);
    }
}
