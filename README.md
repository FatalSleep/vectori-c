# vectori-c
Vector w/ Internal Iterator in C (with option void pointer type). Calling `vectori_free()` will free the underlying vector memory, but NOT any dynamically allocated pointers/items it may store--that needs to be done manually.

`optvoidpntr_t` is a struct which stores info to allocated memory of a `void*`:

```C
// optvoidpntr_t typedef struct.
typedef struct optvoidpntr_t { bool32_t hasValue; void_t* value; } optvoidpntr_t;

// Returns a default struct (zero init) with no pointer allocation.
optvoidpntr_t  optvoidpntr_def();
// Sets the pointer of this optional type.
void_t         optvoidpntr_set(optvoidpntr_t* val, void_t* pntr);
// Returns TRUE if the pointer has memory, else FALSE.
bool32_t       optvoidpntr_has(optvoidpntr_t* val);
// Returns the underlying pointer.
void_t*        optvoidpntr_val(optvoidpntr_t* val);
```

```C
/// Default item count for new vectors.
#define VECTORI_DEFAULT_LENGTH 32

/// Vector with internal iterator that accepts void* (generic) data with byte-size typeSize.
typedef struct vectori {
	optvoidpntr_t data; // Data Pointer
	int32_t typeSize; // Type Size (Byte Length)
	size_t length;   // Current Size (Bytes, not Items)
	size_t iterator; // Current Iterator (Bytes, not Items)
} vectori;

/// Returns a new vector (with memory allocated if reserved is TRUE) with default length vectori_DEFAULT_LENGTH.
vectori vectori_calloc(int32_t typeSize, bool32_t reserve);
/// Returns a new vector (with memory allocated if reserved is TRUE).
vectori vectori_calloc2(int32_t typeSize, size_t length, bool32_t reserve);
/// Returns TRUE if the vector was free'd, else FALSE if the vector passed is not allocated.
void_t vectori_free(vectori* vector);
/// Attempts to resize the vector: Returns true if the vector is allocated (regardless if it was resized), else FALSE.
bool32_t vectori_realloc(vectori* vector, size_t length);
/// Returns TRUE if the vector is allocated, else FALSE.
bool32_t vectori_isalloc(vectori* vector);
/// Returns an optvoidpntr_t (optional void pointer), describing the struct pointer data.
optvoidpntr_t vectori_getpntr(vectori* vector);
/// Returns the full byte-length of allocated memory for a vector.
size_t vectori_bytesize(vectori* vector);
/// Returns the maximum number of elements that can be written (until resized on insert).
size_t vectori_maxsize(vectori* vector);
/// Returns the byte-size of the type used for allocations within a vector.
size_t vectori_typesize(vectori* vector);
/// Returns the last iterator position (same-as item count) of a vector.
size_t vectori_last(vectori* vector);
/// Returns the first position (0) of a vector. [Redundant, Placeholder]
size_t vectori_first(vectori* vector);
/// Returns FALSE if [iterator] is not within bounds length > iterator >= 0, else TRUE and set new iterator position.
bool32_t vectori_move(vectori* vector, size_t iterator);
/// Returns TRUE if the vector can be cleared, else FALSE.
bool32_t vectori_clear(vectori* vector, void_t* data);
/// Returns TRUE if the vector is allocated and the element at [index] can be written, else FALSE.
bool32_t vectori_insert(vectori* vector, void_t* data, size_t index);
/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
bool32_t vectori_replace(vectori* vector, void_t* data, size_t index);
/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
bool32_t vectori_replace_unsafe(vectori* vector, void_t* data, size_t index, size_t byteCount);
/// Returns TRUE if the element at [index] can be removed, else FALSE.
bool32_t vectori_remove(vectori* vector, size_t index);
/// Returns NULL if index is not within bounds iterator > index >= 0 or returns pointer to element in vector.
void_t* vectori_get(vectori* vector, size_t index);
/// Returns FALSE if [index] is not within bounds of iterator > index >= 0, else TRUE. [data] is set to element pointer or NULL.
bool32_t vectori_get2(vectori* vector, size_t index, void_t* data);
/// Returns a pointer to a new string constructor from a vector: outLen can be pointer to get length, or NULL to ignore.
char_t* vectori_makestr(vectori* vector, size_t first, size_t last, size_t* outLen);
```
