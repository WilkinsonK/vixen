#pragma once
#include "nodes.hpp"
#include "tokens.hpp"

namespace vixen::parser {
    using namespace nodes;
    using namespace tokens;

    class Parser {
        protected:
            Lexer lexer;

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
            // Requests the next token from the
            // lexer and rotates the token history.
            virtual void update() = 0;
    };

    class TreeParser : public Parser {
        private:
            Token lexer_ribbon[3];

        public:
            TreeParser() {}
            TreeParser(Lexer lexer) {
                this->lexer = lexer;
                this->update();
                this->update();
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
                this->lexer_ribbon[0] = this->current();
                this->lexer_ribbon[1] = this->next();
                this->lexer_ribbon[2] = this->lexer.next();
            }
    };

    typedef TreeNode(*node_parser)(Parser&);
    TreeNode parse_expr(Parser&);

    // Parse the simplest possible expression
    // nodes.
    TreeNode parse_expr_primitive(Parser& parser) {
        Token current_tk = parser.current();
        parser.update();

        // Determines what kind of primitive value
        // we are trying to parse. We first
        // ensure if the current token is unknown
        // and treat it like it is a name.
        if (tokens_isgeneric(current_tk))
            return node_init_literal("Name", current_tk);

        // Next is determine the numerical tokens.
        // If token is a float token type, return
        // a float node.
        else if (tokens_isfloat(current_tk))
            return node_init_literal("Flt", current_tk);

        // If token is a value that can express an
        // integer value, return an integer node.
        else if (tokens_isinteger(current_tk))
            return node_init_literal("Int", current_tk);

        // Parse our groupings next. We treat
        // groupings as an individual phrase which
        // adds weight, in a sense, to the
        // contained expression.
        else if (current_tk.type == TokenType::PuncLParen) {
            TreeNode expr = parse_expr(parser);
            parser.expect(TokenType::PuncRParen);
            parser.update();
            return expr;

        // Parse strings.
        } else if (current_tk.type == TokenType::StrSingleDbl) {
            TreeNode expr = node_init_literal("Str", parser.current());

            // Update the lexer ribbon, validate
            // that the next token is the closing
            // string char and consume that token.
            parser.update();
            parser.expect(TokenType::StrSingleDbl);
            parser.update();
            return expr;

        // If the current token is a terminator,
        // we continue on to the next statement.
        } else if (current_tk.type == TokenType::PuncTerminator) {
            return node_init_term(current_tk);

        // If an unsupported token is presented,
        // throw an error and bail.
        } else {
            std::cerr
                << "Unexpected token at "
                << "(lineno: " << current_tk.lineno << " col: "
                << current_tk.column << "). Got " << current_tk.symbol
                << std::endl;
            std::exit(1);
        }
    }

    // Parse a binary expression from the next
    // series of series of nodes.
    TreeNode parse_expr_binary(
        Parser& parser,
        vector<TokenType> expects,
        node_parser next) {

        auto exists = [&](TokenType item){
            auto exp = expects;
            return std::find(exp.begin(), exp.end(), item) != exp.end();
        };

        Token    operation;
        TreeNode left, right;

        left = next(parser);
        while (exists(parser.current().type)) {
            operation = parser.current();
            parser.update();
            right = next(parser);
            left  = node_init_binary(operation, left, right);
        }

        return left;
    }

    // A binary expression which defines a
    // multiplicative operation.
    // (*, /, //, ** or %)
    TreeNode parse_expr_multiplicative(Parser& parser) {
        return parse_expr_binary(
            parser,
            {
                TokenType::OperDivide,
                TokenType::OperDivFloor,
                TokenType::OperModulus,
                TokenType::OperStar,
                TokenType::OperPower
            },
            parse_expr_primitive);
    }

    TreeNode parse_expr_additive(Parser& parser) {
        return parse_expr_binary(
            parser,
            {
                TokenType::OperPlus,
                TokenType::OperMinus
            },
            parse_expr_multiplicative);
    }

    TreeNode parse_expr(Parser& parser) {
        return parse_expr_additive(parser);
    }

    TreeNode parse_stmt(Parser& parser) {
        return parse_expr(parser);
    }

    // Creates an AST from the given parser and
    // its internal lexer.
    TreeNode parse(Parser& parser) {
        TreeNode program("Program");
        TreeNode next;

        while (!parser.done()) {
            next = parse_stmt(parser);
            parser.update();
            node_program_add(program, next);
        }

        return program;
    }
};
