#ifndef VIXEN_STRINGS
/*
* Defines dynamic string objects usable for our
* purposes.
*/

#include <string.h>

#include "objects.h"

#define VXNSTR_REF_MAX (1<<15)

typedef struct VxnStr VxnStr;
// Destroy reference table.
static void VxnStrings_Dump();
// Calculate hash value for a VxnStr.
static VxnSize VxnStrings_Hash(VxnStr*);
// Load reference table.
static void VxnStrings_Load();
void VxnStr_Del(VxnStr*);
void VxnStr_Ini(VxnStr*, const char*);
VxnStr* VxnStr_New();
void VxnStr_Set(VxnStr*, const char*);
char* VxnStr_Str(VxnStr*);

typedef struct VxnStr {
    VxnSize ref_count;
    VxnSize size;
    char*   value;
} VxnStr;

void VxnStr_Del(VxnStr* str) {
    VixenPTR_delete(str->value);
    VixenPTR_delete(str);
}

void VxnStr_Ini(VxnStr* str, const char* value) {
    VxnStr_Set(str, value);
}

VxnStr* VxnStr_New() {
    VxnStr* str = VixenPTR_malloc(VxnStr);
    str->ref_count = 0;
    str->size      = 0;
    str->value     = VixenPTR_calloc(char, 0);
    return str;
}

char* VxnStr_Str(VxnStr* str) {
    return str->value;
}

void VxnStr_Set(VxnStr* str, const char* value) {
    VxnSize new_size = strlen(value);
    VxnSize old_size = str->size;

    if (old_size == 0) {
        str->value = VixenSTR_malloc(value);
    } else
    if (old_size != new_size) {
        str->value = VixenSTR_realloc(str, new_size);
    }

    str->size = new_size;
    memcpy(str->value, value, str->size);
}

struct VxnStringsRefs {
    VxnSize count;
    // Both structures point to the same strings
    // but where 'array_list' is used for
    // accessing all non-NULL values in a
    // condensed manner.
    VxnStr*  hash_table[VXNSTR_REF_MAX];
    VxnStr** array_list;
};

static struct VxnStringsRefs VxnStringsRefs;

static void VxnStrings_Dump() {
    int i = 0;
    VxnSize hash;
    VxnStr* str;

    while (i < VxnStringsRefs.count) {
        str  = VxnStringsRefs.array_list[i];
        hash = VxnStrings_Hash(str);

        VxnStringsRefs.array_list[i] = NULL;
        VxnStringsRefs.hash_table[hash] = NULL;

        VixenObject_Del(VxnStr, VxnStringsRefs.array_list[i++]);
    }
}

static VxnSize VxnStrings_Hash(VxnStr* str) {
    VxnSize hash = 5381;
    VxnSize size = str->size;

    while (size--) {
        hash += (hash << 5);
        hash ^= str->value[size];
    }

    return hash;
}

static void VxnStrings_Load() {
    VxnStringsRefs.count = 0;

    for (int i=0; i<VXNSTR_REF_MAX; i++)
        VxnStringsRefs.hash_table[i] = NULL;
}

#define VIXEN_STRINGS
#endif