#ifndef IMPORT_SYSTEM_H
#define IMPORT_SYSTEM_H

#include "symbol_table.h"
#include "../ast/ast.h"
#include <stdbool.h>

// Import types
typedef enum {
    IMPORT_MODULE,          // #include core::io
    IMPORT_FUNCTION,        // #include core::io::print
    IMPORT_MODULE_ALIAS,    // #include core::io as io
    IMPORT_FUNCTION_ALIAS   // #include core::io::print as printf
} ImportType;

// Import entry
typedef struct Import {
    ImportType type;
    char* module_path;      // e.g., "core::io"
    char* function_name;    // e.g., "print" (for function imports)
    char* alias;            // e.g., "printf" (for aliased imports)
    struct Import* next;
} Import;

// Import context
typedef struct ImportContext {
    Import* imports;        // Linked list of imports
    SymbolTable* symbol_table;
    char* current_file;
} ImportContext;

// Function definition (for builtin functions)
typedef struct FunctionDefinition {
    const char* qualified_name;  // e.g., "core::io::print"
    const char* c_function;      // e.g., "echo_print_string"
    const char* return_type;
    const char** param_types;
    int param_count;
} FunctionDefinition;

// Import system functions
ImportContext* import_context_create(SymbolTable* symbol_table);
void import_context_destroy(ImportContext* context);

// Process #include directive
bool import_process_include(ImportContext* context, ASTNode* preprocessor);

// Parse import statement
Import* import_parse_statement(const char* include_line);

// Add symbols based on import
bool import_add_symbols(ImportContext* context, Import* import);

// Builtin function definitions (modules are auto-generated!)
extern const FunctionDefinition BUILTIN_FUNCTIONS[];
extern const int BUILTIN_FUNCTION_COUNT;

// Smart helper functions (no more hardcoded modules!)
bool import_is_builtin_module(const char* module_path);
bool import_function_exists_in_module(const char* module_path, const char* function_name);
const FunctionDefinition* import_find_function(const char* qualified_name);
char* import_extract_module_path(const char* include_line);
char* import_extract_function_name(const char* include_line);
char* import_extract_alias(const char* include_line);

#endif // IMPORT_SYSTEM_H 