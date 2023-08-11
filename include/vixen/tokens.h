#ifndef VIXEN_TOKENS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum memory size allocated to register.
#define VXN_TK_DEF_MAX 512
// Register a new token definition.
#define VXN_TK_DEF_ADD(k, n, v) vxn_tk_def_add(k, n, v)
// Removes a token definition from register.
#define VXN_TK_DEF_POP() vxn_token_defs_pop()

// Supported token types.
typedef enum vxn_tk_kind vxn_tk_kind;
// A single token definition.
typedef struct vxn_tk_def vxn_tk_def;
// Initialize the token definition register.
static void vxn_token_defs_init();
// Checks the register to ensure it is in a valid
// state.
static int vxn_token_defs_precheck();
// Resizes the token definition register.
static void vxn_token_defs_resize();
// Adds a new definition to the register.
static void vxn_token_defs_add(vxn_tk_def*);
// Removes a defintion from the top of the
// register.
static void vxn_token_defs_pop();

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

typedef struct vxn_tk_def {
    vxn_tk_kind kind;
    char*       name;
    char*       value;
} vxn_tk_def;

// Get string representation of a definition.
char* vxn_tk_def_str(vxn_tk_def* def) {
    char* buf = (char*)calloc(128, sizeof(char));
    sprintf(
        buf,
        "[definition][%s: \"%s\"]",
        vxn_tk_kind_str(def->kind),
        def->value);
    return buf;
}

// Creates a new token defintion.
vxn_tk_def* vxn_tk_def_new(
    vxn_tk_kind kind, const char* name, const char* value) {

    // Initialize an empty definition.
    vxn_tk_def* def = (vxn_tk_def*)malloc(sizeof(vxn_tk_def));
    def->kind  = kind;
    def->name  = (char*)malloc(strlen(name));
    def->value = (char*)malloc(strlen(value));

    // Copy arguments into definition values.
    memcpy(def->name, name, strlen(name));
    memcpy(def->value, value, strlen(value));

    return def;
}

// Append new definition to register.
void vxn_tk_def_add(vxn_tk_kind kind, const char* name, const char* value) {
    vxn_tk_def* def = vxn_tk_def_new(kind, name, value);
    vxn_token_defs_add(def);
}

// Remove definition from register.
void vxn_tk_def_del(vxn_tk_def* def) {
    // Remove defintion attributes.
    free(def->name);
    free(def->value);
    // Remove definition itself.
    free(def);
}

// Token definition register.
struct vxn_tk_defs {
    vxn_tk_def** defs;
    int          count;
    int          prev_size;
    int          curr_size;
};
static struct vxn_tk_defs vxn_tk_defs;

static void vxn_token_defs_init() {
    vxn_tk_defs.count = 0;

    // We track 'previous size' to resize the
    // register at the rate of the fibonacci
    // sequence.
    vxn_tk_defs.curr_size = 0;
    vxn_tk_defs.prev_size = 1;
    vxn_tk_defs.defs = (vxn_tk_def**)calloc(vxn_tk_defs.curr_size, sizeof(vxn_tk_def));
}

static void vxn_token_defs_resize() {
    // Calculate new size.
    vxn_tk_defs.curr_size = vxn_tk_defs.curr_size + vxn_tk_defs.prev_size;
    vxn_tk_defs.prev_size = vxn_tk_defs.curr_size - vxn_tk_defs.prev_size;

    // Request new size from system.
    int new_size      = (sizeof(vxn_tk_def) * vxn_tk_defs.curr_size);
    vxn_tk_def** defs = vxn_tk_defs.defs;
    vxn_tk_defs.defs  = (vxn_tk_def**)realloc(defs, new_size);
}

static int vxn_token_defs_precheck() {
    // Panic if token maximum is reached.
    if ((vxn_tk_defs.count + 1) < vxn_tk_defs.curr_size)
        return 1;

    if ((vxn_tk_defs.curr_size + 1) <= VXN_TK_DEF_MAX) {
        vxn_token_defs_resize();
        return 1;
    }

    return 0;
}

static void vxn_token_defs_add(vxn_tk_def* def) {
    if (vxn_token_defs_precheck()) {
        vxn_tk_defs.defs[vxn_tk_defs.count++] = def;
    } else {
        perror("token register reached maximum stack size.");
        exit(EXIT_FAILURE);
    }
}

// Removes a defintion from the top of the
// register.
static void vxn_token_defs_pop() {
    vxn_tk_def* def = vxn_tk_defs.defs[--vxn_tk_defs.count];
    vxn_tk_def_del(def);
}

// Load the token definitions table.
static void vxn_token_load_defs() {
    vxn_token_defs_init();

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
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPAS-SIGN", "=");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPPL-SIGN", "+");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPMS-SIGN", "-");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPMT-SIGN", "*");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPPW-SIGN", "**");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPDV-SIGN", "/");
    VXN_TK_DEF_ADD(TK_OPERATOR, "OPMD-SIGN", "%");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTND-SIGN", "&");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTOR-SIGN", "|");
    VXN_TK_DEF_ADD(TK_OPERATOR, "BTXR-SIGN", "^");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGND-SIGN", "&&");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGOR-SIGN", "||");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGNT-SIGN", "!");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGGT-SIGN", ">");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGLT-SIGN", "<");
    VXN_TK_DEF_ADD(TK_OPERATOR, "LGEQ-SIGN", "==");

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
    while (vxn_tk_defs.count)
        VXN_TK_DEF_POP();
    free(vxn_tk_defs.defs);
}

// typedef struct vxn_tk {
//     int         order;
//     vxn_tk_kind kind;
//     char*       value;
// };

#define VIXEN_TOKENS
#endif