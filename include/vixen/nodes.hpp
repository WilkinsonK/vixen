#pragma once
#include <vector>

#include "tokens.hpp"

namespace vixen::nodes {
    using namespace tokens;

    // A parsed expression, term, or phrase
    // parsed from a sequence of tokens or a
    // value from a single token.
    class TreeNode {
        private:
            std::unordered_map<std::string, TreeNode> children;
            Token       token;
            std::string type;

        public:
            TreeNode() {}
            TreeNode(std::string type) {
                this->type = type;
            }
            TreeNode(std::string type, Token token) {
                this->token = token;
                this->type  = type;
            }

            // Number of child nodes.
            uint child_count() {
                return this->children.size();
            }

            // Get a child node from this node
            // by name.
            TreeNode child_get(std::string name) {
                return this->children[name];
            }

            // Assign a child node by name.
            void child_set(std::string name, TreeNode node) {
                this->children[name] = node;
            }
        private:
            friend std::ostream& operator<<(std::ostream& os, const TreeNode& node) {
                os << node.type << "Node";

                if (node.token.symbol != "") {
                    os << "(" << node.token.symbol << ")";
                }

                if (node.children.size() > 0) {
                    os << "[";
                    for (const auto& child : node.children) {
                        os << child.first << ": " << child.second;
                    }
                    os << "]";
                }

                return os;
            }
    };

    /*
    Tree node typology.
    1. Statements
        i. Expressions
            - Binary; involves a left, right and an operator.
        ii. Literals
            - Identity
            - Float
            - Integer
            - String
    */

    #define NDATTR_BODY  "__body_idx"
    #define NDATTR_VALUE "__value__"
    #define NDATTR_LEFT  NDATTR_VALUE"left"
    #define NDATTR_RIGHT NDATTR_VALUE"right"

    // Adds a node to this program body.
    void node_program_add(TreeNode& program, TreeNode& node) {
        std::string name = NDATTR_BODY + std::to_string(program.child_count());
        program.child_set(name, node);
    }

    // Get the ref of a program node.
    TreeNode node_program_get(TreeNode& program, uint idx) {
        std::string name = NDATTR_BODY + std::to_string(idx);
        return program.child_get(name);
    }

    // Get the left value node from a statement.
    TreeNode node_stmt_getleft(TreeNode& stmt) {
        return stmt.child_get(NDATTR_LEFT);
    }

    // Get the right value node from a statement.
    TreeNode node_stmt_getright(TreeNode& stmt) {
        return stmt.child_get(NDATTR_RIGHT);
    }

    // Get the value node from the statement.
    TreeNode node_stmt_getvalue(TreeNode& stmt) {
        return stmt.child_get(NDATTR_VALUE);
    }

    // Set the left value node of the statement.
    void node_stmt_setleft(TreeNode& stmt, TreeNode& left) {
        stmt.child_set(NDATTR_LEFT, left);
    }

    // Set the right value node of the statement.
    void node_stmt_setright(TreeNode& stmt, TreeNode& right) {
        stmt.child_set(NDATTR_RIGHT, right);
    }

    // Set the value node of a statement.
    void node_stmt_setvalue(TreeNode& stmt, TreeNode& value) {
        stmt.child_set(NDATTR_VALUE, value);
    }

    // Initialize a node as a binary statement.
    TreeNode node_init_binary(
        Token operation,
        TreeNode& left,
        TreeNode& right) {

            TreeNode stmt("BinaryOperation", operation);
            node_stmt_setleft(stmt, left);
            node_stmt_setright(stmt, right);

            return stmt;
        }
};
