#include "echo_runtime.h"

// I/O functions (core::io module)

void echo_print_string(const char* str) {
    if (str) {
        printf("%s\n", str);
    } else {
        printf("(null)\n");
    }
}

void echo_print_int(int32_t value) {
    printf("%d\n", value);
}

void echo_print_int64(int64_t value) {
    printf("%ld\n", value);
}

void echo_print_float(float value) {
    printf("%.6f\n", value);
}

void echo_print_double(double value) {
    printf("%.6f\n", value);
}

void echo_print_bool(bool value) {
    printf("%s\n", value ? "true" : "false");
}

// Memory management functions (core::mem module)

void* echo_alloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Echo Runtime Error: Failed to allocate %zu bytes\n", size);
        exit(1);
    }
    return ptr;
}

void echo_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void* echo_alloc_array(size_t element_size, size_t count) {
    size_t total_size = element_size * count;
    void* ptr = calloc(count, element_size);
    if (!ptr && total_size > 0) {
        fprintf(stderr, "Echo Runtime Error: Failed to allocate array of %zu elements (%zu bytes each)\n", count, element_size);
        exit(1);
    }
    return ptr;
}

void echo_free_array(void* ptr) {
    echo_free(ptr);
}

// String functions

char* echo_string_concat(const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";
    
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    size_t total_len = len_a + len_b + 1;
    
    char* result = echo_alloc(total_len);
    strcpy(result, a);
    strcat(result, b);
    
    return result;
}

char* echo_string_from_int(int32_t value) {
    char* result = echo_alloc(32); // Enough for any 32-bit int
    snprintf(result, 32, "%d", value);
    return result;
}

char* echo_string_from_float(float value) {
    char* result = echo_alloc(64); // Enough for float representation
    snprintf(result, 64, "%.6f", value);
    return result;
}

char* echo_string_from_double(double value) {
    char* result = echo_alloc(64); // Enough for double representation
    snprintf(result, 64, "%.6f", value);
    return result;
}

char* echo_string_from_bool(bool value) {
    const char* str = value ? "true" : "false";
    char* result = echo_alloc(strlen(str) + 1);
    strcpy(result, str);
    return result;
}

// Utility functions

void echo_runtime_init(void) {
    // Initialize runtime if needed
    // For now, nothing to do
}

void echo_runtime_cleanup(void) {
    // Cleanup runtime if needed
    // For now, nothing to do
} 