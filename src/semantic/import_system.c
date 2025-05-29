#define _GNU_SOURCE
#include "import_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Parameter type definitions for builtin functions
static const char* STRING_PARAMS[] = { "string", NULL };
static const char* INT_PARAMS[] = { "i32", NULL };
static const char* SIZE_T_PARAMS[] = { "size_t", NULL };
static const char* PTR_PARAMS[] = { "void*", NULL };

// Builtin function definitions
const FunctionDefinition BUILTIN_FUNCTIONS[] = {
    // core::io module
    {
        .qualified_name = "core::io::print",
        .c_function = "echo_print_string",
        .return_type = "void",
        .param_types = STRING_PARAMS,
        .param_count = 1
    },
    {
        .qualified_name = "core::io::print_int",
        .c_function = "echo_print_int",
        .return_type = "void", 
        .param_types = INT_PARAMS,
        .param_count = 1
    },
    {
        .qualified_name = "core::io::print_bool",
        .c_function = "echo_print_bool",
        .return_type = "void",
        .param_types = (const char*[]){"bool", NULL},
        .param_count = 1
    },
    
    // core::mem module
    {
        .qualified_name = "core::mem::alloc",
        .c_function = "echo_alloc",
        .return_type = "void*",
        .param_types = SIZE_T_PARAMS,
        .param_count = 1
    },
    {
        .qualified_name = "core::mem::free",
        .c_function = "echo_free",
        .return_type = "void",
        .param_types = PTR_PARAMS,
        .param_count = 1
    },
    
    // core::string module
    {
        .qualified_name = "core::string::concat",
        .c_function = "echo_string_concat",
        .return_type = "string",
        .param_types = (const char*[]){"string", "string", NULL},
        .param_count = 2
    },
    {
        .qualified_name = "core::string::from_int",
        .c_function = "echo_string_from_int",
        .return_type = "string",
        .param_types = INT_PARAMS,
        .param_count = 1
    }
};

const int BUILTIN_FUNCTION_COUNT = sizeof(BUILTIN_FUNCTIONS) / sizeof(FunctionDefinition);

// Find function by qualified name
const FunctionDefinition* find_function(const char* qualified_name) {
    if (!qualified_name) return NULL;
    
    for (int i = 0; i < BUILTIN_FUNCTION_COUNT; i++) {
        if (strcmp(BUILTIN_FUNCTIONS[i].qualified_name, qualified_name) == 0) {
            return &BUILTIN_FUNCTIONS[i];
        }
    }
    
    return NULL;
}

// Smart module detection - no more hardcoded BUILTIN_MODULES!
// Modules are auto-generated from BUILTIN_FUNCTIONS

// Check if module is builtin by looking at functions
bool import_is_builtin_module(const char* module_path) {
    if (!module_path) return false;
    
    // Check if any builtin function starts with this module path
    for (int i = 0; i < BUILTIN_FUNCTION_COUNT; i++) {
        if (strncmp(BUILTIN_FUNCTIONS[i].qualified_name, module_path, strlen(module_path)) == 0 &&
            BUILTIN_FUNCTIONS[i].qualified_name[strlen(module_path)] == ':' &&
            BUILTIN_FUNCTIONS[i].qualified_name[strlen(module_path)+1] == ':') {
            return true;
        }
    }
    
    return false;
}

// Check if function exists in module by looking at BUILTIN_FUNCTIONS
bool import_function_exists_in_module(const char* module_path, const char* function_name) {
    if (!module_path || !function_name) return false;
    
    char full_name[256];
    snprintf(full_name, sizeof(full_name), "%s::%s", module_path, function_name);
    
    return find_function(full_name) != NULL;
}

// Helper function to create type node
ASTNode* create_type_node(const char* type_name) {
    if (!type_name) return NULL;
    
    ASTNode* type_node = malloc(sizeof(ASTNode));
    if (!type_node) return NULL;
    
    type_node->type = AST_TYPE;
    type_node->value = strdup(type_name);
    type_node->child_count = 0;
    type_node->children = NULL;
    type_node->line = 0;
    type_node->column = 0;
    type_node->data_type = NULL;
    
    return type_node;
}

// Create import context
ImportContext* import_context_create(SymbolTable* symbol_table) {
    ImportContext* context = malloc(sizeof(ImportContext));
    if (!context) return NULL;
    
    context->imports = NULL;
    context->symbol_table = symbol_table;
    context->current_file = NULL;
    
    return context;
}

// Destroy import context
void import_context_destroy(ImportContext* context) {
    if (!context) return;
    
    // Free imports
    Import* import = context->imports;
    while (import) {
        Import* next = import->next;
        free(import->module_path);
        free(import->function_name);
        free(import->alias);
        free(import);
        import = next;
    }
    
    free(context->current_file);
    free(context);
}

// Process #include directive
bool import_process_include(ImportContext* context, ASTNode* preprocessor) {
    if (!context || !preprocessor || preprocessor->type != AST_PREPROCESSOR) {
        return false;
    }
    
    if (!preprocessor->value || strncmp(preprocessor->value, "#include ", 9) != 0) {
        return true; // Not an include, skip
    }
    
    // Extract include line
    const char* include_line = preprocessor->value + 9; // Skip "#include "
    
    printf("Processing include: %s\n", include_line);
    
    // Parse import statement
    Import* import = import_parse_statement(include_line);
    if (!import) {
        printf("Failed to parse import: %s\n", include_line);
        return false;
    }
    
    // Add to imports list
    import->next = context->imports;
    context->imports = import;
    
    // Add symbols to symbol table
    bool success = import_add_symbols(context, import);
    
    printf("Import processed: %s (type: %d, success: %s)\n", 
           import->module_path, import->type, success ? "yes" : "no");
    
    return success;
}

// Parse import statement
Import* import_parse_statement(const char* include_line) {
    if (!include_line) return NULL;
    
    Import* import = malloc(sizeof(Import));
    if (!import) return NULL;
    
    import->module_path = NULL;
    import->function_name = NULL;
    import->alias = NULL;
    import->next = NULL;
    
    // Remove leading/trailing whitespace
    while (*include_line == ' ' || *include_line == '\t') include_line++;
    
    char* line_copy = strdup(include_line);
    char* line = line_copy;
    
    // Remove trailing whitespace
    int len = strlen(line);
    while (len > 0 && (line[len-1] == ' ' || line[len-1] == '\t' || line[len-1] == '\n')) {
        line[--len] = '\0';
    }
    
    // Check for "as" keyword
    char* as_pos = strstr(line, " as ");
    if (as_pos) {
        *as_pos = '\0';
        import->alias = strdup(as_pos + 4); // Skip " as "
    }
    
    // Parse module path
    import->module_path = strdup(line);
    
    // Determine import type
    char* last_colon = strrchr(import->module_path, ':');
    if (last_colon && last_colon > import->module_path && *(last_colon-1) == ':') {
        // This might be a function import (module::function)
        // Check if the part after :: is a valid function name
        char* potential_function = last_colon + 1;
        
        // Create module path without function
        *(last_colon-1) = '\0'; // Remove ::function part
        
        // Check if this module exists and has this function
        if (import_is_builtin_module(import->module_path)) {
            // Check if function exists in this module using our smart function
            if (import_function_exists_in_module(import->module_path, potential_function)) {
                // It's a function import
                import->function_name = strdup(potential_function);
                import->type = import->alias ? IMPORT_FUNCTION_ALIAS : IMPORT_FUNCTION;
                free(line_copy);
                return import;
            }
        }
        
        // Not a function import, restore the path
        *(last_colon-1) = ':';
    }
    
    // It's a module import
    import->type = import->alias ? IMPORT_MODULE_ALIAS : IMPORT_MODULE;
    
    free(line_copy);
    return import;
}

// Add symbols based on import
bool import_add_symbols(ImportContext* context, Import* import) {
    if (!context || !import) return false;
    
    switch (import->type) {
        case IMPORT_MODULE: {
            // Import all functions from module with module:: prefix
            printf("  Adding module: %s\n", import->module_path);
            
            // Extract short module name (e.g., "io" from "core::io")
            const char* short_name = strrchr(import->module_path, ':');
            if (short_name && short_name > import->module_path && *(short_name-1) == ':') {
                short_name++; // Skip the ::
            } else {
                short_name = import->module_path; // Use full name if no ::
            }
            
            for (int i = 0; i < BUILTIN_FUNCTION_COUNT; i++) {
                const FunctionDefinition* func = &BUILTIN_FUNCTIONS[i];
                
                // Check if function belongs to this module
                if (strncmp(func->qualified_name, import->module_path, strlen(import->module_path)) == 0 &&
                    func->qualified_name[strlen(import->module_path)] == ':' &&
                    func->qualified_name[strlen(import->module_path)+1] == ':') {
                    
                    // Add with full qualified name (core::io::print)
                    ASTNode* return_type = create_type_node(func->return_type);
                    Symbol* symbol = symbol_create(func->qualified_name, SYMBOL_FUNCTION, NULL, return_type);
                    symbol->is_builtin = true;
                    symbol->c_function_name = strdup(func->c_function); // Store C function name
                    
                    if (symbol_table_add_symbol(context->symbol_table, symbol)) {
                        printf("    + %s -> %s\n", func->qualified_name, func->c_function);
                    } else {
                        symbol_destroy(symbol);
                    }
                    
                    // Also add with short module name (io::print)
                    const char* function_part = func->qualified_name + strlen(import->module_path) + 2;
                    char short_name_func[256];
                    snprintf(short_name_func, sizeof(short_name_func), "%s::%s", short_name, function_part);
                    
                    ASTNode* return_type2 = create_type_node(func->return_type);
                    Symbol* symbol2 = symbol_create(short_name_func, SYMBOL_FUNCTION, NULL, return_type2);
                    symbol2->is_builtin = true;
                    symbol2->c_function_name = strdup(func->c_function); // Store C function name
                    
                    if (symbol_table_add_symbol(context->symbol_table, symbol2)) {
                        printf("    + %s -> %s\n", short_name_func, func->c_function);
                    } else {
                        symbol_destroy(symbol2);
                    }
                }
            }
            return true;
        }
        
        case IMPORT_FUNCTION: {
            // Import specific function with short name
            char qualified_name[256];
            snprintf(qualified_name, sizeof(qualified_name), "%s::%s", 
                    import->module_path, import->function_name);
            
            printf("  Adding function: %s as %s\n", qualified_name, import->function_name);
            
            const FunctionDefinition* func = find_function(qualified_name);
            if (func) {
                ASTNode* return_type = create_type_node(func->return_type);
                Symbol* symbol = symbol_create(import->function_name, SYMBOL_FUNCTION, NULL, return_type);
                symbol->is_builtin = true;
                symbol->c_function_name = strdup(func->c_function); // Store C function name
                
                return symbol_table_add_symbol(context->symbol_table, symbol);
            }
            return false;
        }
        
        case IMPORT_FUNCTION_ALIAS: {
            // Import specific function with alias
            char qualified_name[256];
            snprintf(qualified_name, sizeof(qualified_name), "%s::%s", 
                    import->module_path, import->function_name);
            
            printf("  Adding function: %s as %s\n", qualified_name, import->alias);
            
            const FunctionDefinition* func = find_function(qualified_name);
            if (func) {
                ASTNode* return_type = create_type_node(func->return_type);
                Symbol* symbol = symbol_create(import->alias, SYMBOL_FUNCTION, NULL, return_type);
                symbol->is_builtin = true;
                symbol->c_function_name = strdup(func->c_function); // Store C function name
                
                return symbol_table_add_symbol(context->symbol_table, symbol);
            }
            return false;
        }
        
        case IMPORT_MODULE_ALIAS: {
            // Import module with alias (module functions accessible as alias::function)
            printf("  Adding module: %s as %s\n", import->module_path, import->alias);
            
            for (int i = 0; i < BUILTIN_FUNCTION_COUNT; i++) {
                const FunctionDefinition* func = &BUILTIN_FUNCTIONS[i];
                
                // Check if function belongs to this module
                if (strncmp(func->qualified_name, import->module_path, strlen(import->module_path)) == 0 &&
                    func->qualified_name[strlen(import->module_path)] == ':' &&
                    func->qualified_name[strlen(import->module_path)+1] == ':') {
                    
                    // Create aliased name (alias::function instead of module::function)
                    const char* function_part = func->qualified_name + strlen(import->module_path) + 2;
                    char aliased_name[256];
                    snprintf(aliased_name, sizeof(aliased_name), "%s::%s", import->alias, function_part);
                    
                    ASTNode* return_type = create_type_node(func->return_type);
                    Symbol* symbol = symbol_create(aliased_name, SYMBOL_FUNCTION, NULL, return_type);
                    symbol->is_builtin = true;
                    symbol->c_function_name = strdup(func->c_function); // Store C function name
                    
                    if (symbol_table_add_symbol(context->symbol_table, symbol)) {
                        printf("    + %s -> %s\n", aliased_name, func->c_function);
                    } else {
                        symbol_destroy(symbol);
                    }
                }
            }
            return true;
        }
    }
    
    return false;
} 