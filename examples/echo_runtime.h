#ifndef ECHO_RUNTIME_H
#define ECHO_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// Echo runtime library
// Provides basic functionality for Echo programs

// I/O functions (core::io module)
void echo_print_string(const char* str);
void echo_print_int(int32_t value);
void echo_print_int64(int64_t value);
void echo_print_float(float value);
void echo_print_double(double value);
void echo_print_bool(bool value);

// Memory management functions (core::mem module)
void* echo_alloc(size_t size);
void echo_free(void* ptr);
void* echo_alloc_array(size_t element_size, size_t count);
void echo_free_array(void* ptr);

// String functions
char* echo_string_concat(const char* a, const char* b);
char* echo_string_from_int(int32_t value);
char* echo_string_from_float(float value);
char* echo_string_from_double(double value);
char* echo_string_from_bool(bool value);

// Utility functions
void echo_runtime_init(void);
void echo_runtime_cleanup(void);

#endif // ECHO_RUNTIME_H 