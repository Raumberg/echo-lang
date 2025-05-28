#define _GNU_SOURCE
#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Hash table size for symbols
#define SYMBOL_HASH_SIZE 64

// Simple hash function for symbol names
static unsigned int hash_symbol_name(const char* name) {
    if (!name) return 0;
    
    unsigned int hash = 5381;
    for (const char* p = name; *p; p++) {
        hash = ((hash << 5) + hash) + *p;
    }
    return hash % SYMBOL_HASH_SIZE;
}

// Create symbol table
SymbolTable* symbol_table_create(void) {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (!table) return NULL;
    
    table->global_scope = scope_create(NULL, false);
    if (!table->global_scope) {
        free(table);
        return NULL;
    }
    
    table->current_scope = table->global_scope;
    table->scope_counter = 0;
    
    return table;
}

// Destroy symbol table
void symbol_table_destroy(SymbolTable* table) {
    if (!table) return;
    
    scope_destroy(table->global_scope);
    free(table);
}

// Create scope
Scope* scope_create(Scope* parent, bool is_function_scope) {
    Scope* scope = malloc(sizeof(Scope));
    if (!scope) return NULL;
    
    // Initialize hash table
    scope->symbols = calloc(SYMBOL_HASH_SIZE, sizeof(Symbol*));
    if (!scope->symbols) {
        free(scope);
        return NULL;
    }
    
    scope->parent = parent;
    scope->level = parent ? parent->level + 1 : 0;
    scope->is_function_scope = is_function_scope;
    scope->function_node = NULL;
    scope->next = NULL;
    
    return scope;
}

// Destroy scope
void scope_destroy(Scope* scope) {
    if (!scope) return;
    
    // Destroy all symbols in hash table
    for (int i = 0; i < SYMBOL_HASH_SIZE; i++) {
        Symbol* symbol = ((Symbol**)scope->symbols)[i];
        while (symbol) {
            Symbol* next = symbol->next;
            symbol_destroy(symbol);
            symbol = next;
        }
    }
    
    free(scope->symbols);
    
    // Don't recursively destroy child scopes to avoid issues
    // In a full implementation, we'd need proper scope lifetime management
    
    free(scope);
}

// Enter new scope
void symbol_table_enter_scope(SymbolTable* table, bool is_function_scope) {
    if (!table) return;
    
    Scope* new_scope = scope_create(table->current_scope, is_function_scope);
    if (!new_scope) return;
    
    table->current_scope = new_scope;
    table->scope_counter++;
}

// Exit current scope
void symbol_table_exit_scope(SymbolTable* table) {
    if (!table || !table->current_scope || table->current_scope == table->global_scope) {
        return;
    }
    
    Scope* old_scope = table->current_scope;
    table->current_scope = old_scope->parent;
    
    // For now, don't destroy the scope immediately to avoid memory issues
    // In a full implementation, we'd need proper scope lifetime management
}

// Create symbol
Symbol* symbol_create(const char* name, SymbolType type, ASTNode* declaration, ASTNode* type_node) {
    if (!name) return NULL;
    
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) return NULL;
    
    symbol->name = strdup(name);
    symbol->type = type;
    symbol->declaration = declaration;
    symbol->type_node = type_node;
    symbol->scope_level = 0;
    symbol->is_initialized = false;
    symbol->is_used = false;
    symbol->is_parameter = (type == SYMBOL_PARAMETER);
    symbol->next = NULL;
    
    return symbol;
}

// Destroy symbol
void symbol_destroy(Symbol* symbol) {
    if (!symbol) return;
    
    free(symbol->name);
    // Note: We don't destroy AST nodes here as they're owned by the AST
    free(symbol);
}

// Add symbol to current scope
bool symbol_table_add_symbol(SymbolTable* table, Symbol* symbol) {
    if (!table || !symbol || !table->current_scope) return false;
    
    // Check if symbol already exists in current scope
    Symbol* existing = symbol_table_lookup_current_scope(table, symbol->name);
    if (existing) {
        return false; // Symbol already exists
    }
    
    // Add to hash table
    unsigned int hash = hash_symbol_name(symbol->name);
    Symbol** bucket = &((Symbol**)table->current_scope->symbols)[hash];
    
    symbol->scope_level = table->current_scope->level;
    symbol->next = *bucket;
    *bucket = symbol;
    
    return true;
}

// Lookup symbol in current scope only
Symbol* symbol_table_lookup_current_scope(SymbolTable* table, const char* name) {
    if (!table || !name || !table->current_scope) return NULL;
    
    unsigned int hash = hash_symbol_name(name);
    Symbol* symbol = ((Symbol**)table->current_scope->symbols)[hash];
    
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    
    return NULL;
}

// Lookup symbol in current scope and all parent scopes
Symbol* symbol_table_lookup(SymbolTable* table, const char* name) {
    if (!table || !name) return NULL;
    
    Scope* scope = table->current_scope;
    while (scope) {
        unsigned int hash = hash_symbol_name(name);
        Symbol* symbol = ((Symbol**)scope->symbols)[hash];
        
        while (symbol) {
            if (strcmp(symbol->name, name) == 0) {
                symbol->is_used = true; // Mark as used
                return symbol;
            }
            symbol = symbol->next;
        }
        
        scope = scope->parent;
    }
    
    return NULL;
}

// Check if currently in function scope
bool symbol_table_is_in_function_scope(SymbolTable* table) {
    if (!table) return false;
    
    Scope* scope = table->current_scope;
    while (scope) {
        if (scope->is_function_scope) {
            return true;
        }
        scope = scope->parent;
    }
    
    return false;
}

// Get the nearest function scope
Scope* symbol_table_get_function_scope(SymbolTable* table) {
    if (!table) return NULL;
    
    Scope* scope = table->current_scope;
    while (scope) {
        if (scope->is_function_scope) {
            return scope;
        }
        scope = scope->parent;
    }
    
    return NULL;
}

// Convert symbol type to string
const char* symbol_type_to_string(SymbolType type) {
    switch (type) {
        case SYMBOL_VARIABLE: return "variable";
        case SYMBOL_FUNCTION: return "function";
        case SYMBOL_PARAMETER: return "parameter";
        case SYMBOL_TYPE: return "type";
        case SYMBOL_STRUCT: return "struct";
        case SYMBOL_ENUM: return "enum";
        default: return "unknown";
    }
}

// Print symbol table (for debugging)
void symbol_table_print(SymbolTable* table) {
    if (!table) return;
    
    printf("=== Symbol Table ===\n");
    
    Scope* scope = table->current_scope;
    int scope_num = 0;
    
    while (scope) {
        printf("Scope %d (level %d, %s):\n", 
               scope_num++, scope->level, 
               scope->is_function_scope ? "function" : "block");
        
        for (int i = 0; i < SYMBOL_HASH_SIZE; i++) {
            Symbol* symbol = ((Symbol**)scope->symbols)[i];
            while (symbol) {
                printf("  %s: %s", symbol->name, symbol_type_to_string(symbol->type));
                if (symbol->type_node && symbol->type_node->value) {
                    printf(" (%s)", symbol->type_node->value);
                }
                if (symbol->is_initialized) printf(" [initialized]");
                if (symbol->is_used) printf(" [used]");
                printf("\n");
                symbol = symbol->next;
            }
        }
        
        scope = scope->parent;
        if (scope) printf("\n");
    }
    
    printf("==================\n");
} 