#ifndef ECHO_RUNTIME_H
#define ECHO_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Optional type implementation
#define ECHO_OPTIONAL(T) struct { bool has_value; T value; }
#define ECHO_SOME(val) { .has_value = true, .value = (val) }
#define ECHO_NONE { .has_value = false }
#define ECHO_IS_SOME(opt) ((opt).has_value)
#define ECHO_IS_NONE(opt) (!(opt).has_value)
#define ECHO_UNWRAP(opt) ((opt).value)
#define ECHO_UNWRAP_OR(opt, default_val) ((opt).has_value ? (opt).value : (default_val))

// Common optional types
typedef ECHO_OPTIONAL(int32_t) echo_optional_i32;
typedef ECHO_OPTIONAL(int64_t) echo_optional_i64;
typedef ECHO_OPTIONAL(float) echo_optional_f32;
typedef ECHO_OPTIONAL(double) echo_optional_f64;
typedef ECHO_OPTIONAL(bool) echo_optional_bool;
typedef ECHO_OPTIONAL(char*) echo_optional_string;

// Smart pointer types
typedef struct echo_unique_ptr {
    void* ptr;
    void (*destructor)(void*);
} echo_unique_ptr;

typedef struct echo_shared_ptr {
    void* ptr;
    int* ref_count;
    void (*destructor)(void*);
} echo_shared_ptr;

// Unique pointer functions
echo_unique_ptr* echo_unique_create(size_t size, void (*destructor)(void*));
echo_unique_ptr* echo_unique_create_value(void* value, void (*destructor)(void*));
void echo_unique_destroy(echo_unique_ptr* ptr);
void* echo_unique_get(echo_unique_ptr* ptr);
void* echo_unique_release(echo_unique_ptr* ptr);
bool echo_unique_is_valid(echo_unique_ptr* ptr);

// Shared pointer functions
echo_shared_ptr* echo_shared_create(size_t size, void (*destructor)(void*));
echo_shared_ptr* echo_shared_create_value(void* value, void (*destructor)(void*));
echo_shared_ptr* echo_shared_copy(echo_shared_ptr* ptr);
void echo_shared_destroy(echo_shared_ptr* ptr);
void* echo_shared_get(echo_shared_ptr* ptr);
int echo_shared_ref_count(echo_shared_ptr* ptr);
bool echo_shared_is_valid(echo_shared_ptr* ptr);

// Memory allocation wrappers
void* echo_alloc(size_t size);
void* echo_calloc(size_t count, size_t size);
void* echo_realloc(void* ptr, size_t size);
void echo_free(void* ptr);

// String utilities
char* echo_string_create(const char* str);
char* echo_string_concat(const char* str1, const char* str2);
char* echo_string_substring(const char* str, int start, int length);
int echo_string_length(const char* str);
bool echo_string_equals(const char* str1, const char* str2);
void echo_string_destroy(char* str);

// Array utilities
typedef struct echo_array {
    void* data;
    size_t length;
    size_t capacity;
    size_t element_size;
    void (*destructor)(void*);
} echo_array;

echo_array* echo_array_create(size_t element_size, void (*destructor)(void*));
void echo_array_destroy(echo_array* arr);
bool echo_array_push(echo_array* arr, void* element);
bool echo_array_pop(echo_array* arr, void* out_element);
void* echo_array_get(echo_array* arr, size_t index);
bool echo_array_set(echo_array* arr, size_t index, void* element);
size_t echo_array_length(echo_array* arr);

// Error handling
typedef enum {
    ECHO_ERROR_NONE = 0,
    ECHO_ERROR_NULL_POINTER,
    ECHO_ERROR_OUT_OF_MEMORY,
    ECHO_ERROR_INDEX_OUT_OF_BOUNDS,
    ECHO_ERROR_INVALID_ARGUMENT,
    ECHO_ERROR_DIVISION_BY_ZERO
} echo_error_t;

extern echo_error_t echo_last_error;

void echo_set_error(echo_error_t error);
echo_error_t echo_get_error(void);
const char* echo_error_string(echo_error_t error);
void echo_clear_error(void);

// Panic and assertion
void echo_panic(const char* message);
void echo_assert(bool condition, const char* message);

// Debug utilities
void echo_debug_print(const char* format, ...);
void echo_debug_print_optional_i32(echo_optional_i32 opt);
void echo_debug_print_unique_ptr(echo_unique_ptr* ptr);
void echo_debug_print_shared_ptr(echo_shared_ptr* ptr);

// Runtime initialization and cleanup
void echo_runtime_init(void);
void echo_runtime_cleanup(void);

// Type-specific destructors
void echo_destructor_i32(void* ptr);
void echo_destructor_i64(void* ptr);
void echo_destructor_f32(void* ptr);
void echo_destructor_f64(void* ptr);
void echo_destructor_bool(void* ptr);
void echo_destructor_string(void* ptr);

// Utility macros
#define ECHO_ARRAY_FOREACH(arr, type, var) \
    for (size_t _i = 0; _i < echo_array_length(arr); _i++) { \
        type* var = (type*)echo_array_get(arr, _i); \
        if (var) {

#define ECHO_ARRAY_FOREACH_END } }

#define ECHO_UNIQUE_CAST(ptr, type) ((type*)echo_unique_get(ptr))
#define ECHO_SHARED_CAST(ptr, type) ((type*)echo_shared_get(ptr))

// Version information
#define ECHO_RUNTIME_VERSION_MAJOR 1
#define ECHO_RUNTIME_VERSION_MINOR 0
#define ECHO_RUNTIME_VERSION_PATCH 0

const char* echo_runtime_version(void);

#endif // ECHO_RUNTIME_H 