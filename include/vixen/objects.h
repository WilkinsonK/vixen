#ifndef VIXEN_OBJECTS
/*
* Defines common types and macros shared among
* Vixen's objects.
*/

#include <stdlib.h>

// Delete a given vixen object by calling it's
// `_Del` function.
#define VixenObject_Del(T, PTR) T##_Del(PTR)
// Initilize an instance of type `T` by calling
// it's `_Init` function.
#define VixenObject_Ini(T, PTR, ...) T##_Ini(PTR, __VA_ARGS__)
// Create an instance of type `T` by calling
// it's `_New` function. Creates a raw instance,
// does not populate other than default 'known'
// values.
#define VixenObject_New(T) T##_New()
// Returns the string representation of the `T`
// instance by calling it's `_Str` function.
#define VixenObject_Str(T, PTR) T##_Str(PTR)
// Allocates memory on the heap of size `SZ` as a
// pointer of type `T`.
#define VixenPTR_calloc(T, SZ) (T*)calloc(SZ, sizeof(T))
// Deallocate memory on the heap for pointer.
#define VixenPTR_delete(PTR) free(PTR)
// Allocates memory on the heap as a pointer of
// type `T`.
#define VixenPTR_malloc(T) (T*)malloc(sizeof(T))
// Reallocate the memory space on the heap for a
// pointer of type `T`.
#define VixenPTR_realloc(T, PTR, SZ) (T*)realloc(PTR, SZ)
// Allocates memory on the heap, as a dynamic
// char array, large enough to hold `STR`.
#define VixenSTR_malloc(STR) (char*)malloc(strlen(STR))
// Reallocate the memory space on the heap for a
// dynamic string.
#define VixenSTR_realloc(STR, SZ) (char*)realloc(STR, SZ)
typedef unsigned long long VxnSize;

#define VIXEN_OBJECTS
#endif