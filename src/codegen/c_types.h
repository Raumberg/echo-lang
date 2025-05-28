#ifndef C_TYPES_H
#define C_TYPES_H

#include <stdbool.h>

// Type mapping utilities
typedef struct {
    const char* echo_type;
    const char* c_type;
    bool is_primitive;
    bool needs_include;
    const char* include_header;
} TypeMapping;

// Built-in type mappings
extern const TypeMapping builtin_types[];
extern const int builtin_types_count;

// Type conversion functions
const char* c_types_echo_to_c(const char* echo_type);
const char* c_types_get_default_value(const char* c_type);
bool c_types_is_numeric(const char* echo_type);
bool c_types_is_integer(const char* echo_type);
bool c_types_is_floating(const char* echo_type);
bool c_types_is_signed(const char* echo_type);

// Optional type utilities
bool c_types_is_optional(const char* echo_type);
char* c_types_extract_optional_base_type(const char* echo_type);
char* c_types_generate_optional_type_name(const char* base_type);

// Pointer type utilities
bool c_types_is_pointer(const char* echo_type);
char* c_types_extract_pointer_base_type(const char* echo_type);

// Smart pointer utilities
bool c_types_is_unique_ptr(const char* echo_type);
bool c_types_is_shared_ptr(const char* echo_type);
char* c_types_extract_smart_ptr_base_type(const char* echo_type);

// Array type utilities
bool c_types_is_array(const char* echo_type);
char* c_types_extract_array_base_type(const char* echo_type);
int c_types_extract_array_size(const char* echo_type);

// Function type utilities
bool c_types_is_function_type(const char* echo_type);
char* c_types_extract_return_type(const char* echo_type);
char** c_types_extract_parameter_types(const char* echo_type, int* count);

// Type compatibility checking
bool c_types_are_compatible(const char* type1, const char* type2);
bool c_types_can_cast(const char* from_type, const char* to_type);
const char* c_types_get_cast_expression(const char* from_type, const char* to_type, const char* value);

// Size and alignment
size_t c_types_get_size(const char* echo_type);
size_t c_types_get_alignment(const char* echo_type);

// Include requirements
bool c_types_needs_stdint(const char* echo_type);
bool c_types_needs_stdbool(const char* echo_type);
bool c_types_needs_stdlib(const char* echo_type);
bool c_types_needs_string(const char* echo_type);

// Helper functions
char* c_types_sanitize_identifier(const char* identifier);
bool c_types_is_valid_c_identifier(const char* identifier);
char* c_types_escape_string_literal(const char* str);

// Memory management
void c_types_free_string(char* str);

#endif // C_TYPES_H 