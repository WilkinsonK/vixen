#ifndef VIXEN_TOKENS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum memory size allocated to register.
#define VXN_TK_DEF_MAX 512

// Vixen token.
typedef struct VxnTk VxnTk;
// Supported token types.
typedef enum VxnTkKind VxnTkKind;
// A single token definition.
typedef struct VxnTkDef VxnTkDef;
// Initialize token definitions.
static void VxnTokens_LoadDefs();
// Destroy token definitions.
static void VxnTokens_DumpDefs();
// Initialize the token definition register.
static void VxnTokenDefs_Init();
// Checks the register to ensure it is in a valid
// state.
static int VxnTokenDefs_PreCheck();
// Resizes the token definition register.
static void VxnTokenDefs_Resize();
// Adds a new definition to the register.
static void VxnTokenDefs_Add(VxnTkKind, const char*, const char*);
// Removes a defintion from the top of the
// register.
static void VxnTokenDefs_Pop();

typedef enum VxnTkKind {
    Tk_ERROR,
    Tk_FLOAT,
    Tk_IDENTIFIER,
    Tk_INTEGER,
    Tk_KEYWORD,
    Tk_NEWLINE,
    Tk_OPERATOR,
    Tk_STRING,
    Tk_SYNTAX,
    Tk_TYPE
} VxnTkKind;

// Get string representation of a token kind.
char* VxnTkKind_Str(VxnTkKind k) {
    switch (k) {
        case Tk_ERROR:
            return "err";
        case Tk_FLOAT:
            return "float";
        case Tk_IDENTIFIER:
            return "id";
        case Tk_INTEGER:
            return "int";
        case Tk_KEYWORD:
            return "kwd";
        case Tk_NEWLINE:
            return "newline";
        case Tk_OPERATOR:
            return "op";
        case Tk_STRING:
            return "str";
        case Tk_SYNTAX:
            return "syntax";
        case Tk_TYPE:
            return "type";
        default:
            return "unk";
    }
}

typedef struct VxnTkDef {
    VxnTkKind kind;
    char*       name;
    char*       value;
} VxnTkDef;

// Get string representation of a definition.
char* VxnTkDef_Str(VxnTkDef* def) {
    char* buf = (char*)calloc(128, sizeof(char));
    sprintf(
        buf,
        "[definition][%s: \"%s\"]",
        VxnTkKind_Str(def->kind),
        def->value);
    return buf;
}

// Creates a new token defintion.
VxnTkDef* VxnTkDef_New(VxnTkKind kind, const char* name, const char* value) {

    // Initialize an empty definition.
    VxnTkDef* def = (VxnTkDef*)malloc(sizeof(VxnTkDef));
    def->kind  = kind;
    def->name  = (char*)malloc(strlen(name));
    def->value = (char*)malloc(strlen(value));

    // Copy arguments into definition values.
    memcpy(def->name, name, strlen(name));
    memcpy(def->value, value, strlen(value));

    return def;
}

// Remove definition from register.
void VxnTkDef_Del(VxnTkDef* def) {
    // Remove defintion attributes.
    free(def->name);
    free(def->value);
    // Remove definition itself.
    free(def);
}

// Token definition register.
struct VxnTkDefs {
    VxnTkDef** defs;
    int        count;
    int        prev_size;
    int        curr_size;
};
static struct VxnTkDefs VxnTkDefs;

static void VxnTokenDefs_Init() {
    VxnTkDefs.count = 0;

    // We track 'previous size' to resize the
    // register at the rate of the fibonacci
    // sequence.
    VxnTkDefs.curr_size = 0;
    VxnTkDefs.prev_size = 1;
    VxnTkDefs.defs = (VxnTkDef**)calloc(VxnTkDefs.curr_size, sizeof(VxnTkDef));
}

static void VxnTokenDefs_Resize() {
    // Calculate new size.
    VxnTkDefs.curr_size = VxnTkDefs.curr_size + VxnTkDefs.prev_size;
    VxnTkDefs.prev_size = VxnTkDefs.curr_size - VxnTkDefs.prev_size;

    // Request new size from system.
    int new_size      = (sizeof(VxnTkDef) * VxnTkDefs.curr_size);
    VxnTkDef** defs = VxnTkDefs.defs;
    VxnTkDefs.defs  = (VxnTkDef**)realloc(defs, new_size);
}

static int VxnTokenDefs_PreCheck() {
    // Panic if token maximum is reached.
    if ((VxnTkDefs.count + 1) < VxnTkDefs.curr_size)
        return 1;

    if ((VxnTkDefs.curr_size + 1) <= VXN_TK_DEF_MAX) {
        VxnTokenDefs_Resize();
        return 1;
    }

    return 0;
}

static void VxnTokenDefs_Add(
    VxnTkKind kind, const char* name, const char* value) {

    if (VxnTokenDefs_PreCheck()) {
        VxnTkDefs.defs[VxnTkDefs.count++] = VxnTkDef_New(kind, name, value);
    } else {
        perror("token register reached maximum stack size.");
        exit(EXIT_FAILURE);
    }
}

static void VxnTokenDefs_Pop() {
    VxnTkDef_Del(VxnTkDefs.defs[--VxnTkDefs.count]);
}

static void VxnTokens_LoadDefs() {
    VxnTokenDefs_Init();

    // Define float pattern.
    VxnTokenDefs_Add(Tk_FLOAT, "FLOATING-NUMBER", "\\d+\\.\\d+");
    // Define identifier pattern.
    VxnTokenDefs_Add(Tk_IDENTIFIER, "IDENTIFIER", "\\w+");

    // Define integer pattern.
    VxnTokenDefs_Add(Tk_INTEGER, "INTEGER-NUMBER", "\\d+");

    // Define keyword patterns.
    VxnTokenDefs_Add(Tk_KEYWORD, "DEFINE", "def");
    VxnTokenDefs_Add(Tk_KEYWORD, "DEFINE-CLASS", "class");
    VxnTokenDefs_Add(Tk_KEYWORD, "NEW", "new");

    // Define newline pattern.
    VxnTokenDefs_Add(Tk_NEWLINE, "NEWLINE", "\\n");

    // Define operator patthers.
    VxnTokenDefs_Add(Tk_OPERATOR, "OPAS-SIGN", "=");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPPL-SIGN", "+");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMS-SIGN", "-");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMT-SIGN", "*");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPPW-SIGN", "**");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPDV-SIGN", "/");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMD-SIGN", "%");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTND-SIGN", "&");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTOR-SIGN", "|");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTXR-SIGN", "^");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGND-SIGN", "&&");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGOR-SIGN", "||");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGNT-SIGN", "!");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGGT-SIGN", ">");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGLT-SIGN", "<");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGEQ-SIGN", "==");

    VxnTokenDefs_Add(Tk_STRING, "DBL-QUOTE", "\".*\"");
    VxnTokenDefs_Add(Tk_STRING, "SGL-QUOTE", "'.*'");
    VxnTokenDefs_Add(Tk_STRING, "DBLDOC-QUOTE", "\"\"\".*\"\"\"");
    VxnTokenDefs_Add(Tk_STRING, "SGLDOC-QUOTE", "'''.*'''");
    VxnTokenDefs_Add(Tk_STRING, "COMMENT", "#.*");
    VxnTokenDefs_Add(Tk_STRING, "INLINE-COMMENT", "##.*##");

    // Define syntax patterns.
    VxnTokenDefs_Add(Tk_SYNTAX, "L-BRACKET", "[");
    VxnTokenDefs_Add(Tk_SYNTAX, "R-BRACKET", "]");
    VxnTokenDefs_Add(Tk_SYNTAX, "L-BRACE", "{");
    VxnTokenDefs_Add(Tk_SYNTAX, "R-BRACE", "}");
    VxnTokenDefs_Add(Tk_SYNTAX, "L-PAREN", "(");
    VxnTokenDefs_Add(Tk_SYNTAX, "R-PAREN", ")");
    VxnTokenDefs_Add(Tk_SYNTAX, "COMMA", ",");
    VxnTokenDefs_Add(Tk_SYNTAX, "SEMI-COLON", ";");
    VxnTokenDefs_Add(Tk_SYNTAX, "COLON", ":");
    VxnTokenDefs_Add(Tk_SYNTAX, "DOT", ".");
    VxnTokenDefs_Add(Tk_SYNTAX, "STAMP", "@");

    // Define type patterns.
    VxnTokenDefs_Add(Tk_TYPE, "FLT-TYPE", "flt");
    VxnTokenDefs_Add(Tk_TYPE, "INT-TYPE", "int");
    VxnTokenDefs_Add(Tk_TYPE, "STR-TYPE", "str");
    VxnTokenDefs_Add(Tk_TYPE, "BYT-TYPE", "byt");
}

// Perform cleanup tasks on token definitions
// table.
static void VxnTokens_DumpDefs() {
    while (VxnTkDefs.count)
        VxnTokenDefs_Pop();
    free(VxnTkDefs.defs);
}

typedef struct vxn_tk {
    int       order;
    VxnTkKind kind;
    char*     value;
} vxn_tk;

#define VIXEN_TOKENS
#endif