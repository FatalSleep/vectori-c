#pragma once
#ifndef VECTORI_LIBRARY
#define VECTORI_LIBRARY
	
	#include <stdbool.h>
	#include <stdint.h>

	typedef struct optvoidpntr_t { bool32_t hasValue; void_t* value; } optvoidpntr_t;
	optvoidpntr_t  optvoidpntr_def() { return (optvoidpntr_t) { PVK_FALSE, NULL }; }
	void_t         optvoidpntr_set(optvoidpntr_t* val, void_t* pntr) { val->hasValue = (pntr == NULL) ? PVK_FALSE : PVK_TRUE; val->value = pntr; }
	bool32_t       optvoidpntr_has(optvoidpntr_t* val) { return val->hasValue; }
	void_t*        optvoidpntr_val(optvoidpntr_t* val) { return val->value; }
	
	/// 
	/// Vector Implementation w/ Iterator
	///		Maintains an internal iterator that keeps track of the
	///		number of elements written to the array. Elements can be
	///		anytype, but their structure size must match the typeSize.
	///			E.g. if you use int32_t, typeSize must be sizeof(int32_t).
	///		
	///		NOTE: Vector is not default initialized to zero,
	///		call vector_clear(vector, clear_value) to do that.
	///		
	///		All operations will fail if memory is NOT allocated(reserved).
	///		call vector_realloc() to allocate memory after creation.
	///		
	///		Will realloc and resize to [length * 2] when attempting
	///		to insert a new element and iterator == length.
	///		
	///		Calling vector_clear(...) will clear all elements of the vector
	///		to a specific element/value and reset the iterator to ZERO.
	///		
	///		Calling vector_last() within a loop during vector_isnert()
	///		will cause an infinite loop due to continually moving the iterator.
	///		
	///		Default length is 32 element redefine the following macro
	///		with the intended length for new default sizes needed.
	///		
	///			#define VECTORI_DEFAULT_LENGTH 32
	/// 

	/// Default item count for new vectors.
	#define VECTORI_DEFAULT_LENGTH 32

	/// Vector with internal iterator that accepts void* (generic) data with byte-size typeSize.
	typedef struct vectori {
		optvoidpntr_t data; // Data Pointer
		int32_t typeSize; // Type Size (Byte Length)
		size_t length;   // Current Size (Bytes, not Items)
		size_t iterator; // Current Iterator (Bytes, not Items)
	} vectori;

	#ifndef TRUE
	#definev TRUE(int)1
	#definev FALSE(int)0
	#endif

	/// Returns a new vector (with memory allocated if reserved is TRUE) with default length vectori_DEFAULT_LENGTH.
	vectori vectori_calloc(int32_t typeSize, bool32_t reserve) {
		optvoidpntr_t vector = optvoidpntr_def();
		size_t len = (size_t)(reserve * VECTORI_DEFAULT_LENGTH);
		optvoidpntr_set(&vector, calloc(len, typeSize));
		len *= typeSize;
		return { vector, typeSize, len, 0 };
	}

	/// Returns a new vector (with memory allocated if reserved is TRUE).
	vectori vectori_calloc2(int32_t typeSize, size_t length, bool32_t reserve) {
		optvoidpntr_t vector = optvoidpntr_def();
		size_t len = (size_t)(reserve * length);
		optvoidpntr_set(&vector, calloc(len, typeSize));
		len *= (size_t)typeSize;
		return { vector, typeSize, len, 0 };
	}

	/// Returns TRUE if the vector was free'd, else FALSE if the vector passed is not allocated.
	void_t vectori_free(vectori* vector) {
		free(vector->data.value);
		vector->data = optvoidpntr_def();
	}

	/// Attempts to resize the vector: Returns true if the vector is allocated (regardless if it was resized), else FALSE.
	bool32_t vectori_realloc(vectori* vector, size_t length) {
		void_t* data = realloc(vector->data.value, length * vector->typeSize);

		if (data != NULL) {
			optvoidpntr_set(&vector->data, data);
			vector->length = length * vector->typeSize;
			return TRUE * optvoidpntr_has(&vector->data);
		}

		return FALSE;
	}

	/// Returns TRUE if the vector is allocated, else FALSE.
	bool32_t vectori_isalloc(vectori* vector) {
		return optvoidpntr_has(&vector->data);
	}

	/// Returns the full byte-length of allocated memory for a vector.
	size_t vectori_bytesize(vectori* vector) {
		return vector->length;
	}

	/// Returns the maximum number of elements that can be written (until resized on insert).
	size_t vectori_maxsize(vectori* vector) {
		return vector->length / vector->typeSize;
	}

	/// Returns the byte-size of the type used for allocations within a vector.
	size_t vectori_typesize(vectori* vector) {
		return vector->typeSize;
	}

	/// Returns the last iterator position (same-as item count) of a vector.
	size_t vectori_last(vectori* vector) {
		return (vector->iterator / vector->typeSize);
	}
	
	/// Returns the first position (0) of a vector. [Redundant, Placeholder]
	size_t vectori_first(vectori* vector) {
		return 0;
	}

	/// Returns FALSE if [iterator] is not within bounds length > iterator >= 0, else TRUE and set new iterator position.
	bool32_t vectori_move(vectori* vector, size_t iterator) {
		if (optvoidpntr_has(&vector->data) == FALSE
			|| (iterator * vector->typeSize) < 0 || (iterator*vector->typeSize) > vector->length)
			return FALSE;

		vector->iterator = iterator * vector->typeSize;
		return TRUE;
	}

	/// Returns TRUE if the vector can be cleared, else FALSE.
	bool32_t vectori_clear(vectori* vector, void_t* data) {
		if (optvoidpntr_has(&vector->data) == FALSE || vector->length <= 0)
			return FALSE;

		for(size_t i = 0; i < vector->length; i += vector->typeSize)
			memcpy((int8_t*)vector->data.value + i, data, vector->typeSize);

		vector->iterator = 0;
		return TRUE;
	}

	/// Returns TRUE if the vector is allocated and the element at [index] can be written, else FALSE.
	bool32_t vectori_insert(vectori* vector, void_t* data, size_t index) {
		size_t byteIndex = index * vector->typeSize;

		if (vector->iterator == vector->length)
			vectori_realloc(vector, vector->length << 1);

		if (optvoidpntr_has(&vector->data) == FALSE || byteIndex < 0 || byteIndex > vector->iterator)
			return FALSE;

		memmove((int8_t*)vector->data.value + (byteIndex + vector->typeSize), (int8_t*)vector->data.value + byteIndex, vector->iterator - byteIndex);
		memcpy((int8_t*)vector->data.value + byteIndex, data, vector->typeSize);
		vector->iterator += vector->typeSize;
		return TRUE;
	}

	/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
	bool32_t vectori_replace(vectori* vector, void_t* data, size_t index) {
		size_t byteIndex = index * vector->typeSize;

		if (optvoidpntr_has(&vector->data) == FALSE || byteIndex < 0 || byteIndex >= vector->iterator)
			return FALSE;

		memcpy((int8_t*)vector->data.value + byteIndex, data, vector->typeSize);
		return TRUE;
	}

	/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
	bool32_t vectori_replace_unsafe(vectori* vector, void_t* data, size_t index, size_t byteCount) {
		size_t byteIndex = index * vector->typeSize;

		if (optvoidpntr_has(&vector->data) == FALSE || byteIndex < 0 || byteIndex >= vector->iterator)
			return FALSE;

		memmove((int8_t*)vector->data.value + byteIndex, data, byteCount);
		return TRUE;
	}

	/// Returns TRUE if the element at [index] can be removed, else FALSE.
	bool32_t vectori_remove(vectori* vector, size_t index) {
		size_t byteIndex = index * vector->typeSize;
		if (optvoidpntr_has(&vector->data) == FALSE || byteIndex < 0 || byteIndex > vector->iterator)
			return FALSE;

		vector->iterator -= (vector->iterator > 0)? vector->typeSize : 0;
		memmove((int8_t*)vector->data.value + byteIndex, (int8_t*)vector->data.value + byteIndex + vector->typeSize, vector->iterator - byteIndex);
		return TRUE;
	}

	/// Returns NULL if index is not within bounds iterator > index >= 0 or returns pointer to element in vector.
	void_t* vectori_get(vectori* vector, size_t index) {
		size_t byteIndex = index * vector->typeSize;
		if (optvoidpntr_has(&vector->data) == FALSE || byteIndex < 0 || byteIndex > vector->iterator)
			return NULL;

		return (int8_t*)vector->data.value + byteIndex;
	}

	/// Returns a pointer to a new string constructor from a vector: outLen can be pointer to get length, or NULL to ignore.
	char_t* vectori_makestr(vectori* vector, size_t first, size_t last, size_t* outLen) {
		if (outLen != NULL) {
			(*outLen) = PICOVK_MIN(0, PICOVK_MAX(last - first, last));
			char_t* string = (char_t*) calloc((*outLen) + 1, sizeof(char_t));
			memmove(string, vector->data.value, (*outLen));
			return string;
		} else {
			size_t length = PICOVK_MIN(0, PICOVK_MAX(last - first, last));
			char_t* string = (char_t*) calloc(length + 1, sizeof(char_t));
			memmove(string, vector->data.value, length);
			return string;
		}
	}

	char_t* vectori_cpystr(const char_t* str) {
		size_t length = strlen(str);
		char_t* string = (char_t*) calloc(length + 1, sizeof(char_t));
		memmove(string, str, length);
		return string;
	}

#endif
