#pragma once
#include "nodes.hpp"
#include "tokens.hpp"

namespace vixen::parser {
    using namespace nodes;
    using namespace tokens;

    class Parser {
        protected:
            Lexer       lexer;
            ProgramNode program;

        public:
            // The current token being observed.
            virtual Token current() = 0;
            // The last token observed.
            virtual Token previous() = 0;
            // The next token to be observed.
            virtual Token next() = 0;
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
            virtual void expect(TokenType type) = 0;
            // Parses the tokens provided by the
            // lexer.
            virtual void parse() = 0;
            // Requests the next token from the
            // lexer and rotates the token history.
            virtual void update() = 0;
    };

    class TreeParser : public Parser {
        private:
            Token lexer_ribbon[3];

        public:
            TreeParser(Lexer lexer) {
                this->lexer = lexer;
                this->lexer_ribbon[1] = lexer.next();
                this->lexer_ribbon[2] = lexer.next();
                this->program         = ProgramNode();
            }

            Token current() {
                return this->lexer_ribbon[1];
            }

            Token previous() {
                return this->lexer_ribbon[0];
            }

            Token next() {
                return this->lexer_ribbon[2];
            }

            bool done() {
                for (const auto& symbol : {"EOF", "EOL"}) {
                    if (this->current().symbol == symbol)
                        return true;
                }
                return false;
            }

            void expect(TokenType type) {
                std::string got, exp;
                Token curr;
                
                curr = this->current();
                if (curr.type != type) {
                    got = tokens_find_genname(curr.symbol);
                    exp = tokens_find_genname(type);
                    std::cerr
                        << "Expected " << exp << " got '" << got << "'."
                        << std::endl;
                    exit(1);
                }
            }

            void update() {
                this->lexer_ribbon[0] = this->lexer_ribbon[1];
                this->lexer_ribbon[1] = this->lexer_ribbon[2];
                this->lexer_ribbon[2] = this->lexer.next();
            }

            void parse() {
                while (!this->done()) {
                    this->program.add(this->parse_stmt());
                    this->update();
                }
            }

            StatementNode parse_stmt() {}
    };
};