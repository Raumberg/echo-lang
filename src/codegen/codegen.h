#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdbool.h>
#include "../ast/ast.h"
#include "../semantic/semantic.h"

// Forward declarations
typedef struct CodeGenerator CodeGenerator;

// Code generator structure
struct CodeGenerator {
    FILE* output;                    // Output C file
    SymbolTable* symbol_table;       // Symbol information from semantic analysis
    int indent_level;                // Current indentation level
    bool in_function;                // Are we currently inside a function?
    char* current_function_name;     // Name of current function being generated
    int temp_var_counter;            // Counter for generating temporary variables
    int label_counter;               // Counter for generating unique labels
    bool has_main_function;          // Does the program have a main function?
    bool needs_runtime;              // Does generated code need runtime support?
};

// Code generation result
typedef enum {
    CODEGEN_SUCCESS,
    CODEGEN_ERROR_FILE_IO,
    CODEGEN_ERROR_INVALID_AST,
    CODEGEN_ERROR_UNSUPPORTED_FEATURE,
    CODEGEN_ERROR_MEMORY_ALLOCATION
} CodegenResult;

// Main interface functions
CodeGenerator* codegen_create(FILE* output, SymbolTable* symbol_table);
void codegen_destroy(CodeGenerator* gen);

// Core code generation
CodegenResult codegen_generate(CodeGenerator* gen, ASTNode* ast);
CodegenResult codegen_generate_program(CodeGenerator* gen, ASTNode* program);

// Function generation
CodegenResult codegen_generate_function(CodeGenerator* gen, ASTNode* function);
CodegenResult codegen_generate_function_signature(CodeGenerator* gen, ASTNode* function);
CodegenResult codegen_generate_function_body(CodeGenerator* gen, ASTNode* function);

// Statement generation
CodegenResult codegen_generate_statement(CodeGenerator* gen, ASTNode* stmt);
CodegenResult codegen_generate_block(CodeGenerator* gen, ASTNode* block);
CodegenResult codegen_generate_variable_decl(CodeGenerator* gen, ASTNode* var_decl);
CodegenResult codegen_generate_return(CodeGenerator* gen, ASTNode* return_stmt);
CodegenResult codegen_generate_if(CodeGenerator* gen, ASTNode* if_stmt);
CodegenResult codegen_generate_for(CodeGenerator* gen, ASTNode* for_stmt);
CodegenResult codegen_generate_while(CodeGenerator* gen, ASTNode* while_stmt);

// Expression generation
CodegenResult codegen_generate_expression(CodeGenerator* gen, ASTNode* expr);
CodegenResult codegen_generate_binary_op(CodeGenerator* gen, ASTNode* binary_op);
CodegenResult codegen_generate_unary_op(CodeGenerator* gen, ASTNode* unary_op);
CodegenResult codegen_generate_call(CodeGenerator* gen, ASTNode* call);
CodegenResult codegen_generate_identifier(CodeGenerator* gen, ASTNode* identifier);
CodegenResult codegen_generate_literal(CodeGenerator* gen, ASTNode* literal);
CodegenResult codegen_generate_assignment(CodeGenerator* gen, ASTNode* assignment);

// Type conversion utilities
const char* codegen_echo_type_to_c_type(const char* echo_type);
bool codegen_is_optional_type(const char* echo_type);
bool codegen_is_pointer_type(const char* echo_type);
bool codegen_is_smart_pointer_type(const char* echo_type);

// Helper functions
void codegen_write_indent(CodeGenerator* gen);
void codegen_increase_indent(CodeGenerator* gen);
void codegen_decrease_indent(CodeGenerator* gen);
void codegen_write_line(CodeGenerator* gen, const char* format, ...);
void codegen_write(CodeGenerator* gen, const char* format, ...);
char* codegen_generate_temp_var(CodeGenerator* gen);
char* codegen_generate_label(CodeGenerator* gen);

// Header generation
CodegenResult codegen_generate_includes(CodeGenerator* gen);
CodegenResult codegen_generate_type_definitions(CodeGenerator* gen);
CodegenResult codegen_generate_function_declarations(CodeGenerator* gen, ASTNode* program);

// Runtime support
CodegenResult codegen_generate_runtime_support(CodeGenerator* gen);
bool codegen_needs_optional_support(ASTNode* ast);
bool codegen_needs_smart_pointer_support(ASTNode* ast);

// Error handling
const char* codegen_result_to_string(CodegenResult result);

#endif // CODEGEN_H 