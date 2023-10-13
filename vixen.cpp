#include <fstream>
#include <iostream>

#include "include/vixen.hpp"

using namespace std;

int main(void) {
    std::ifstream file("tests/test_symbols.vxn");
    vixen::symbols::Lexer lexer(file);
    uint lineno, column;
    std::string token;

    while (!lexer.end()) {
        std::tie(lineno, column, token) = lexer.next();
        std::cout
            << "Token['" << token << "']"
            << "@(lineno: " << lineno << ", col: " << column << ")"
            << std::endl;
    }

    file.close();

    return 0;
}
