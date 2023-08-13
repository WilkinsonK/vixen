#ifndef VXN_LEXER
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

// Lexical analyzer. Creates Tokens from the text
// given to it.
typedef struct VxnLxr VxnLxr;
// Create tokens from the given text.
static VxnTk** VxnLexer_AnalyzeText(const char* text);
// Create a new lexer.
static VxnLxr* VxnLxr_New(const char* text);
// Destroy the given lexer.
static VxnLxr* VxnLxr_Del(VxnLxr* lxr);
// Proceed to the next char position.
static void VxnLxr_Advance(VxnLxr* lxr);
// Create tokens from the given lexer's text.
static void VxnLxr_MakeAllTokens(VxnLxr* lxr);
// Creates the next available token from lexer's
// text.
static void VxnLxr_MakeToken(VxnLxr* lxr);

typedef struct VxnLxr {
    char*     text;
    char*     current_char;
    long long position;
} VxnLxr;

static VxnTk** VxnLexer_AnalyzeText(const char* text) {
    VxnLxr* lxr = VxnLxr_New(text);

    VxnLxr_Del(lxr);
}

static VxnLxr* VxnLxr_New(const char* text) {
    VxnLxr* lxr = (VxnLxr*)malloc(sizeof(VxnLxr));
    lxr->current_char = NULL;
    lxr->position = -1;
    lxr->text = (char*)malloc(strlen(text));
    memcpy(lxr->text, text, strlen(text));

    return lxr;
}

static VxnLxr* VxnLxr_Del(VxnLxr* lxr) {
    free(lxr->text);
    free(lxr);
}

static void VxnLxr_Advance(VxnLxr* lxr) {
    lxr->position++;
    if (lxr->position >= strlen(lxr->text)) {
        lxr->current_char = NULL;
        return;
    }

    lxr->current_char = &(lxr->text[lxr->position]);
}

static void VxnLxr_MakeAllTokens(VxnLxr* lxr) {

}

static void VxnLxr_MakeToken(VxnLxr* lxr) {

}

#define VXN_LEXER
#endif