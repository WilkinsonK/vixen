#ifndef VIXEN_LEXER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Supported token types.
typedef enum vxn_tk_kind {
    TK_ERROR,
    TK_FLOAT,
    TK_IDENTIFIER,
    TK_INTEGER,
    TK_KEYWORD,
    TK_NEWLINE,
    TK_OPERATOR,
    TK_STRING,
    TK_SYNTAX,
    TK_TYPE
} vxn_tk_kind;

// Get string representation of a token kind.
char* vxn_tk_kind_str(vxn_tk_kind k) {
    switch (k) {
        case TK_ERROR:
            return "err";
        case TK_FLOAT:
            return "float";
        case TK_IDENTIFIER:
            return "id";
        case TK_INTEGER:
            return "int";
        case TK_KEYWORD:
            return "kwd";
        case TK_NEWLINE:
            return "newline";
        case TK_OPERATOR:
            return "op";
        case TK_STRING:
            return "str";
        case TK_SYNTAX:
            return "syntax";
        case TK_TYPE:
            return "type";
        default:
            return "unk";
    }
}

// A single token definition.
typedef struct vxn_tk_def {
    vxn_tk_kind kind;
    char*       name;
    char*       value;
} vxn_tk_def;

// Maximum memory size allocated to register.
#define VXN_TK_DEF_MAX 512
// Register a new token definition.
#define VXN_TK_DEF_ADD(k, n, v) vxn_tk_def_add(k, n, v)
// Removes a token definition from register.
#define VXN_TK_DEF_POP() vxn_tk_def_del(vxn_tk_defs[--vxn_tk_def_counter])
// Tracks the number of registered token
// definitions.
static int vxn_tk_def_counter = 0;
// Token definition register.
static vxn_tk_def** vxn_tk_defs;

// Get string representation of a definition.
char* vxn_tk_def_str(vxn_tk_def* def) {
    char* buf = (char*)calloc(128, sizeof(char));
    sprintf(buf, "[definition][%s: '%s']", vxn_tk_kind_str(def->kind), def->value);
    return buf;
}

// Append new definition to register.
void vxn_tk_def_add(vxn_tk_kind kind, const char* name, const char* value) {
    // Initialize an empty definition.
    vxn_tk_def* def = (vxn_tk_def*)malloc(sizeof(vxn_tk_def));
    def->kind = kind;
    def->name = (char*)malloc(strlen(name));
    def->value = (char*)malloc(strlen(value));

    // Copy arguments into definition values.
    memcpy(def->name, name, strlen(name));
    memcpy(def->value, value, strlen(value));

    char* defstr = vxn_tk_def_str(def);
    printf("creating %s @ %p\n", defstr, def);
    free(defstr);

    // append definition to register.
    vxn_tk_defs[vxn_tk_def_counter++] = def;
}

// Remove definition from register.
void vxn_tk_def_del(vxn_tk_def* def) {
    char* defstr = vxn_tk_def_str(def);
    printf("removing %s @ %p\n", defstr, def);
    free(defstr);

    // Remove defintion attributes.
    free(def->name);
    free(def->value);
    // Remove definition itself.
    free(def);
}

// Load the token definitions table.
static void vxn_token_load_defs() {
    vxn_tk_defs = (vxn_tk_def**)calloc(VXN_TK_DEF_MAX, sizeof(vxn_tk_def));

    // Define float pattern.
    VXN_TK_DEF_ADD(TK_FLOAT, "FLOATING-NUMBER", "\\d+\\.\\d+");
    // Define identifier pattern.
    VXN_TK_DEF_ADD(TK_IDENTIFIER, "IDENTIFIER", "\\w+");

    // Define integer pattern.
    VXN_TK_DEF_ADD(TK_INTEGER, "INTEGER-NUMBER", "\\d+");

    // Define keyword patterns.
    VXN_TK_DEF_ADD(TK_KEYWORD, "DEFINE", "def");
    VXN_TK_DEF_ADD(TK_KEYWORD, "DEFINE-CLASS", "class");
    VXN_TK_DEF_ADD(TK_KEYWORD, "NEW", "new");

    // Define newline pattern.
    VXN_TK_DEF_ADD(TK_NEWLINE, "NEWLINE", "\\n");

    // Define operator patthers.
    VXN_TK_DEF_ADD(TK_OPERATOR, "PLUS-SIGN", "+");
    VXN_TK_DEF_ADD(TK_OPERATOR, "MINS-SIGN", "-");
    VXN_TK_DEF_ADD(TK_OPERATOR, "MULT-SIGN", "*");
    VXN_TK_DEF_ADD(TK_OPERATOR, "POWR-SIGN", "**");
    VXN_TK_DEF_ADD(TK_OPERATOR, "DIVI-SIGN", "/");
    VXN_TK_DEF_ADD(TK_OPERATOR, "MODU-SIGN", "%");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTND-SIGN", "&");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTOR-SIGN", "|");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTXR-SIGN", "^");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGND-SIGN", "&&");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGOR-SIGN", "||");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGNT-SIGN", "!");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGGT-SIGN", ">");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGLT-SIGN", "<");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGEQ-SIGN", "==");
    VXN_TK_DEF_ADD(TK_OPERATOR, "ASSIGNMENT", "=");

    VXN_TK_DEF_ADD(TK_STRING, "DBL-QUOTE", "\".*\"");
    VXN_TK_DEF_ADD(TK_STRING, "SGL-QUOTE", "'.*'");
    VXN_TK_DEF_ADD(TK_STRING, "DBLDOC-QUOTE", "\"\"\".*\"\"\"");
    VXN_TK_DEF_ADD(TK_STRING, "SGLDOC-QUOTE", "'''.*'''");
    VXN_TK_DEF_ADD(TK_STRING, "COMMENT", "#.*");
    VXN_TK_DEF_ADD(TK_STRING, "INLINE-COMMENT", "##.*##");

    // Define syntax patterns.
    VXN_TK_DEF_ADD(TK_SYNTAX, "L-BRACKET", "[");
    VXN_TK_DEF_ADD(TK_SYNTAX, "R-BRACKET", "]");
    VXN_TK_DEF_ADD(TK_SYNTAX, "L-BRACE", "{");
    VXN_TK_DEF_ADD(TK_SYNTAX, "R-BRACE", "}");
    VXN_TK_DEF_ADD(TK_SYNTAX, "L-PAREN", "(");
    VXN_TK_DEF_ADD(TK_SYNTAX, "R-PAREN", ")");
    VXN_TK_DEF_ADD(TK_SYNTAX, "COMMA", ",");
    VXN_TK_DEF_ADD(TK_SYNTAX, "SEMI-COLON", ";");
    VXN_TK_DEF_ADD(TK_SYNTAX, "COLON", ":");
    VXN_TK_DEF_ADD(TK_SYNTAX, "DOT", ".");
    VXN_TK_DEF_ADD(TK_SYNTAX, "STAMP", "@");

    // Define type patterns.
    VXN_TK_DEF_ADD(TK_TYPE, "FLT-TYPE", "flt");
    VXN_TK_DEF_ADD(TK_TYPE, "INT-TYPE", "int");
    VXN_TK_DEF_ADD(TK_TYPE, "STR-TYPE", "str");
    VXN_TK_DEF_ADD(TK_TYPE, "BYT-TYPE", "byt");
}

// Perform cleanup tasks on token definitions
// table.
static void vxn_token_dump_defs() {
    while (vxn_tk_def_counter)
        VXN_TK_DEF_POP();
    free(vxn_tk_defs);
}

// typedef struct vxn_token {
//     vxn_token*  parent;
//     vxn_token** children;
//
//     int   order;
//     char* value;
//     char* kind;
// };

#define VIXEN_LEXER
#endif