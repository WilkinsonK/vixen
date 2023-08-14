#ifndef VIXEN_TOKENS
#include <math.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objects.h"

// Maximum memory size allocated to register.
#define VXN_TK_DEF_MAX 512

// Vixen token.
typedef struct VxnTk VxnTk;
// A single token definition.
typedef struct VxnTkDef VxnTkDef;
// Supported token types.
typedef enum VxnTkKind VxnTkKind;
// Linked list containing tokens.
typedef struct VxnTkList VxnTkList;
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
// Create an empty list node.
VxnTkList* VxnTkList_New();
// Initialize a new token list.
void VxnTkList_Init(VxnTkList*, const char*, VxnTkList*);
// Destroy the given list and all of it's
//relatives.
void VxnTkList_Del(VxnTkList*);
// Find the list node at the given index.
VxnTkList* VxnTkList_Find(VxnTkList*, int);
// Insert a token at the end of the list.
void VxnTkList_Append(VxnTkList*, VxnTkList*);
// Remove the child node from the linked list.
void VxnTkList_Remove(VxnTkList*, VxnTkList*);

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
    char*     name;
    char*     value;
    regex_t   value_reg;
} VxnTkDef;

// Get string representation of a definition.
char* VxnTkDef_Str(VxnTkDef* this) {
    char* buf = VixenPTR_calloc(char, 128);
    sprintf(
        buf,
        "[definition][%s: \"%s\"]",
        VxnTkKind_Str(this->kind),
        this->value);
    return buf;
}

// Creates a new token defintion.
VxnTkDef* VxnTkDef_New() {
    // Initialize an empty definition.
    return VixenPTR_malloc(VxnTkDef);
}

// Initializes a new token definition.
void VxnTkDef_Init(
    VxnTkDef* this, VxnTkKind kind, const char* name, const char* value) {

    this->kind  = kind;
    this->name  = VixenSTR_malloc(name);
    this->value = VixenSTR_malloc(value);

    // Copy arguments into definition values.
    memcpy(this->name, name, strlen(name));
    memcpy(this->value, value, strlen(value));

    if (regcomp(&(this->value_reg), this->value, REG_EXTENDED) != 0) {
        fprintf(
            stderr,
            "error: could not compile token regex. '%s'\n",
            this->value);
        exit(EXIT_FAILURE);
    }
}

// Remove definition from register.
void VxnTkDef_Del(VxnTkDef* this) {
    // Remove defintion attributes.
    free(this->name);
    free(this->value);
    regfree(&(this->value_reg));
    // Remove definition itself.
    free(this);
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
    VxnTkDefs.defs = VixenPTR_calloc(VxnTkDef*, VxnTkDefs.curr_size);
}

static void VxnTokenDefs_Resize() {
    // Calculate new size.
    VxnTkDefs.curr_size = VxnTkDefs.curr_size + VxnTkDefs.prev_size;
    VxnTkDefs.prev_size = VxnTkDefs.curr_size - VxnTkDefs.prev_size;

    // Request new size from system.
    int new_size   = (sizeof(VxnTkDef) * VxnTkDefs.curr_size);
    VxnTkDefs.defs = VixenPTR_realloc(VxnTkDef*, VxnTkDefs.defs, new_size);
}

static int VxnTokenDefs_PreCheck() {
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
        VxnTkDef* def = VixenObject_New(VxnTkDef);
        VxnTkDef_Init(def, kind, name, value);
        VxnTkDefs.defs[VxnTkDefs.count++] = def;
    } else {
        fprintf(stderr, "error: token register reached maximum stack size.\n");
        exit(EXIT_FAILURE);
    }
}

static void VxnTokenDefs_Pop() {
    VxnTkDef* def = VxnTkDefs.defs[--VxnTkDefs.count];
    VixenObject_Del(VxnTkDef, def);
}

static void VxnTokens_LoadDefs() {
    VxnTokenDefs_Init();

    // Define error definition.
    VxnTokenDefs_Add(Tk_ERROR, "ERROR", "");

    // Define float pattern.
    VxnTokenDefs_Add(Tk_FLOAT, "FLT-NUMBER", "\\d+\\.\\d+");

    // Define identifier pattern.
    VxnTokenDefs_Add(Tk_IDENTIFIER, "IDENTIFIER", "\\w+");

    // Define integer pattern.
    VxnTokenDefs_Add(Tk_INTEGER, "BIN-NUMBER", "0b\\d+");
    VxnTokenDefs_Add(Tk_INTEGER, "HEX-NUMBER", "0x\\d+");
    VxnTokenDefs_Add(Tk_INTEGER, "INT-NUMBER", "\\d+");
    VxnTokenDefs_Add(Tk_INTEGER, "OCT-NUMBER", "0o\\d+");

    // Define keyword patterns.
    VxnTokenDefs_Add(Tk_KEYWORD, "IF-COND", "if");
    VxnTokenDefs_Add(Tk_KEYWORD, "ELSE-COND", "else");
    VxnTokenDefs_Add(Tk_KEYWORD, "ELIF-COND", "elif");
    VxnTokenDefs_Add(Tk_KEYWORD, "WHILE-COND", "while");
    VxnTokenDefs_Add(Tk_KEYWORD, "FOR-COND", "for");
    VxnTokenDefs_Add(Tk_KEYWORD, "DEFINE", "def");
    VxnTokenDefs_Add(Tk_KEYWORD, "DEFINE-CLASS", "class");
    VxnTokenDefs_Add(Tk_KEYWORD, "NEW", "new");

    // Define newline pattern.
    VxnTokenDefs_Add(Tk_NEWLINE, "NEWLINE", "\\n");

    // Define operator patterns.
    VxnTokenDefs_Add(Tk_OPERATOR, "OPAS-SIGN", "=");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPPL-SIGN", "\\+");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMS-SIGN", "-");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMT-SIGN", "\\*");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPPW-SIGN", "\\*\\*");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPDV-SIGN", "/");
    VxnTokenDefs_Add(Tk_OPERATOR, "OPMD-SIGN", "%");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTND-SIGN", "&");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTOR-SIGN", "\\|");
    VxnTokenDefs_Add(Tk_OPERATOR, "BTXR-SIGN", "^");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGND-SIGN", "&&");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGOR-SIGN", "\\|\\|");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGNT-SIGN", "!");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGGT-SIGN", ">");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGLT-SIGN", "<");
    VxnTokenDefs_Add(Tk_OPERATOR, "LGEQ-SIGN", "==");

    // Define string patterns.
    VxnTokenDefs_Add(Tk_STRING, "DBL-QUOTE", "\"");
    VxnTokenDefs_Add(Tk_STRING, "SGL-QUOTE", "'");
    VxnTokenDefs_Add(Tk_STRING, "DBLDOC-QUOTE", "\"\"\"");
    VxnTokenDefs_Add(Tk_STRING, "SGLDOC-QUOTE", "'''");
    VxnTokenDefs_Add(Tk_STRING, "COMMENT", "#");
    VxnTokenDefs_Add(Tk_STRING, "INLINE-COMMENT", "##");

    // Define syntax patterns.
    VxnTokenDefs_Add(Tk_SYNTAX, "L-BRACKET", "\\[");
    VxnTokenDefs_Add(Tk_SYNTAX, "R-BRACKET", "]");
    VxnTokenDefs_Add(Tk_SYNTAX, "L-BRACE", "\{");
    VxnTokenDefs_Add(Tk_SYNTAX, "R-BRACE", "}");
    VxnTokenDefs_Add(Tk_SYNTAX, "L-PAREN", "\\(");
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

typedef struct VxnTk {
    int       order;
    VxnTkDef* def;
    char*     value;
} VxnTk;

char* VxnTk_Str(VxnTk* this) {
    char* buf = VixenPTR_calloc(char, 128);
    sprintf(
        buf,
        "[%s][%s: '%s']",
        VxnTkKind_Str(this->def->kind),
        this->def->name,
        this->value);
    return buf;
}

VxnTk* VxnTk_New() {
    return VixenPTR_malloc(VxnTk);
}

void VxnTk_Init(VxnTk* this, int order, VxnTkDef* def, const char* value) {
    this->order = order;
    this->def   = def;
    this->value = VixenSTR_malloc(value);
    memcpy(this->value, value, strlen(value));
}

void VxnTk_Del(VxnTk* this) {
    // We don't have to remove the definition as
    // it is destroyed when _DumpDefs is called.
    free(this->value);
    free(this);
}

typedef struct VxnTkList {
    VxnTkList* head;
    VxnTkList* tail;
    VxnTkList* next;
    VxnTkList* prev;
    int        index;
    int        count;
    VxnTk*     token;
} VxnTkList;

VxnTkList* VxnTkList_New() {
    VxnTkList* this = VixenPTR_malloc(VxnTkList);
    this->head = this;
    this->tail = NULL;
    this->next = NULL;
    this->prev = NULL;
    this->index = -1;
    this->count = -1;
    this->token = NULL;
    return this;
}

void VxnTkList_Init(VxnTkList* this, const char* value, VxnTkList* head) {
    // Parse token definition from value.
    // Create token and set to this node.

    // If `head` is NULL we can exit from here.
    // Make sure to set index and count
    // regardless as we will treat this as the
    // head of the list.
    if (head == NULL) {
        this->index = 0;
        this->count = 1;
        this->tail  = this;
        return;
    }

    VxnTkList_Append(head, this);
}

void VxnTkList_Del(VxnTkList* this) {
    VxnTkList* head = this->head;
    VxnTkList* next = head->next;
    while (next != NULL) {
        VixenObject_Del(VxnTk, head->token);
        free(head);

        head = next;
        next = head->next;
    }
}

VxnTkList* VxnTkList_FindFromHead(VxnTkList* this, int index) {
    VxnTkList* node = this->head;
    while (node != NULL && node->index < index) {
        node = node->next;
    }
    return node;
}

VxnTkList* VxnTkList_FindFromTail(VxnTkList* this, int index) {
    VxnTkList* node = this->tail;
    while (node != NULL && node->index > index) {
        node = node->prev;
    }
    return node;
}

VxnTkList* VxnTkList_Find(VxnTkList* this, int index) {
    if (index == -1)
        return this->head->tail;
    if (index == 0)
        return this->head;

    int offset;
    VxnTkList* head = this->head;
    VxnTkList* temp;

    // We want to determine if it is faster to
    // start from the tail end of the list or the
    // head to find the item at this index.
    offset = -1 * (floor(head->count / 2) - (index + 1));
    if (offset <= 0)
        temp = VxnTkList_FindFromHead(this, index);
    else
        temp = VxnTkList_FindFromTail(this, index);

    return temp;
}

void VxnTkList_Append(VxnTkList* this, VxnTkList* that) {
    that->head = this;
    that->head->count++;
    that->index = ++that->head->index;

    that->prev = that->head->tail;
    that->prev->next = that;
    that->head->tail = that;
}

#define VIXEN_TOKENS
#endif