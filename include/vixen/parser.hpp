#pragma once
#include "nodes.hpp"
#include "tokens.hpp"

namespace vixen::parser {
    class Parser {
        private:
            vixen::tokens::Lexer      lexer;
            vixen::nodes::ProgramNode program;

        public:
            // The current token being observed.
            virtual vixen::tokens::Token current() = 0;
            // The last token observed.
            virtual vixen::tokens::Token previous() = 0;
            // The next token to be observed.
            virtual vixen::tokens::Token next() = 0;
            // The lexer has been exhausted of all
            // available tokens.
            virtual bool done() = 0;
            // Looks ahead for the next available
            // token, validating if said token if
            // of the expected type.
            //
            // This function returns `void` but
            // panics if the next token is not of
            // the expected type.
            virtual void expect(vixen::tokens::TokenType type) = 0;
            // Parses the tokens provided by the
            // lexer.
            virtual void parse() = 0;
            // Requests the next token from the
            // lexer and rotates the token history.
            virtual void update() = 0;
    };
};