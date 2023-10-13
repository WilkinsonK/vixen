#pragma once
#include "tokens.hpp"

namespace vixen::nodes {
    // A parsed expression, term, or phrase
    // parsed from a sequence of tokens or a
    // value from a single token.
    class TreeNode {};

    class ProgramNode : public TreeNode {
        private:
            std::vector<TreeNode> body;

        public:
            // Adds a node to this program body.
            void add(TreeNode node) {
                this->body.push_back(node);
            }
    };

    // A phrase composed of multiple nodes.
    class StatementNode : public TreeNode {
        protected:
            vixen::tokens::TokenType type;

        public:
            StatementNode() {}
            StatementNode(vixen::tokens::TokenType type) {
                this->type = type;
            }

            vixen::tokens::TokenType get_type() {
                return this->type;
            }
    };

    // A node which depends on two or more values
    // (e.g. "1 + 2" or "x = y").
    class ExpressionNode : public StatementNode {};

    // A node which depends on three child nodes.
    // A left, an operator, and a right.
    class BinaryExpressionNode : public ExpressionNode {
        private:
            vixen::tokens::Token oper;
            ExpressionNode       left;
            ExpressionNode       right;

        public:
            BinaryExpressionNode(
                vixen::tokens::Token token,
                ExpressionNode left,
                ExpressionNode right) {
                
                this->type  = token.type;
                this->oper  = token;
                this->left  = left;
                this->right = right;
            }
    };

    // A literal value.
    class LiteralNode : public ExpressionNode {
        private:
            vixen::tokens::Token value;

        public:
            LiteralNode(vixen::tokens::Token value) {
                this->type  = value.type;
                this->value = value;
            }
    };

    // A literal expression which identifies
    // another instance.
    class LiteralIdentNode : public LiteralNode {};
    // A literal numerical value.
    class LiteralNumericNode : public LiteralNode {};
    // A literal floating number value.
    class LiteralFltNode : public LiteralNumericNode {};
    // A literal numerical value which digests to
    // an integer.
    class LiteralIntNode : public LiteralNumericNode {};
};
