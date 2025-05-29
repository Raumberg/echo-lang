#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../ast/ast.h"
#include <stdbool.h>

// Symbol types
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_TYPE,
    SYMBOL_STRUCT,
    SYMBOL_ENUM
} SymbolType;

// Symbol structure
typedef struct Symbol {
    char* name;
    SymbolType type;
    ASTNode* declaration;      // AST node where symbol is declared
    ASTNode* ast_node;         // Alias for declaration (for generics compatibility)
    ASTNode* type_node;        // Type information
    char* c_function_name;     // C function name for builtin functions
    int scope_level;
    bool is_initialized;
    bool is_used;
    bool is_parameter;
    bool is_builtin;           // True if this is a builtin symbol
    struct Symbol* next;       // For hash table chaining
} Symbol;

// Scope structure
typedef struct Scope {
    Symbol* symbols;           // Hash table of symbols
    struct Scope* parent;      // Parent scope
    int level;                 // Scope nesting level
    bool is_function_scope;    // True if this is a function scope
    ASTNode* function_node;    // Function AST node (if function scope)
    struct Scope* next;        // For scope stack
} Scope;

// Symbol table structure
typedef struct SymbolTable {
    Scope* current_scope;      // Current active scope
    Scope* global_scope;       // Global scope
    int scope_counter;         // For generating unique scope IDs
} SymbolTable;

// Function declarations

// Symbol table management
SymbolTable* symbol_table_create(void);
void symbol_table_destroy(SymbolTable* table);

// Scope management
Scope* scope_create(Scope* parent, bool is_function_scope);
void scope_destroy(Scope* scope);
void symbol_table_enter_scope(SymbolTable* table, bool is_function_scope);
void symbol_table_exit_scope(SymbolTable* table);

// Symbol management
Symbol* symbol_create(const char* name, SymbolType type, ASTNode* declaration, ASTNode* type_node);
void symbol_destroy(Symbol* symbol);
bool symbol_table_add_symbol(SymbolTable* table, Symbol* symbol);
Symbol* symbol_table_lookup(SymbolTable* table, const char* name);
Symbol* symbol_table_lookup_current_scope(SymbolTable* table, const char* name);

// Utility functions
void symbol_table_print(SymbolTable* table);
const char* symbol_type_to_string(SymbolType type);
bool symbol_table_is_in_function_scope(SymbolTable* table);
Scope* symbol_table_get_function_scope(SymbolTable* table);

#endif // SYMBOL_TABLE_H 