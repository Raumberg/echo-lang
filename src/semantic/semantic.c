#define _GNU_SOURCE
#include "semantic.h"
#include "semantic_errors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Create semantic context
SemanticContext* semantic_create(void) {
    SemanticContext* context = malloc(sizeof(SemanticContext));
    if (!context) return NULL;
    
    context->symbol_table = symbol_table_create();
    if (!context->symbol_table) {
        free(context);
        return NULL;
    }
    
    context->current_function = NULL;
    context->errors = NULL;
    context->error_count = 0;
    context->warning_count = 0;
    context->has_fatal_error = false;
    context->current_filename = NULL;
    
    return context;
}

// Destroy semantic context
void semantic_destroy(SemanticContext* context) {
    if (!context) return;
    
    symbol_table_destroy(context->symbol_table);
    semantic_error_destroy(context->errors);
    free(context->current_filename);
    free(context);
}

// Add error to context
void semantic_add_error(SemanticContext* context, SemanticErrorType type, 
                       SemanticSeverity severity, int line, int column, 
                       const char* format, ...) {
    if (!context) return;
    
    // Format message
    char message[512];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Create error
    SemanticError* error = semantic_error_create(type, severity, line, column, message);
    if (!error) return;
    
    // Add to error list
    error->next = context->errors;
    context->errors = error;
    
    // Update counters
    if (severity == SEMANTIC_SEVERITY_ERROR) {
        context->error_count++;
        if (type == SEMANTIC_ERROR_TYPE_MISMATCH || 
            type == SEMANTIC_ERROR_UNDEFINED_SYMBOL ||
            type == SEMANTIC_ERROR_REDEFINED_SYMBOL) {
            context->has_fatal_error = true;
        }
    } else if (severity == SEMANTIC_SEVERITY_WARNING) {
        context->warning_count++;
    }
}

// Print all errors
void semantic_print_errors(SemanticContext* context) {
    if (!context || !context->errors) return;
    
    printf("\n=== Semantic Analysis Results ===\n");
    
    SemanticError* error = context->errors;
    while (error) {
        printf("%s:%d:%d: %s: %s\n",
               context->current_filename ? context->current_filename : "<unknown>",
               error->line, error->column,
               semantic_severity_to_string(error->severity),
               error->message);
        error = error->next;
    }
    
    printf("\nSummary: %d errors, %d warnings\n", 
           context->error_count, context->warning_count);
}

// Check if context has errors
bool semantic_has_errors(SemanticContext* context) {
    return context && context->error_count > 0;
}

// Main semantic analysis function
bool semantic_analyze(SemanticContext* context, ASTNode* ast) {
    if (!context || !ast) return false;
    
    printf("Starting semantic analysis...\n");
    
    bool success = semantic_analyze_program(context, ast);
    
    if (context->error_count > 0 || context->warning_count > 0) {
        semantic_print_errors(context);
    }
    
    if (success && context->error_count == 0) {
        printf("✓ Semantic analysis completed successfully!\n");
        return true;
    } else {
        printf("✗ Semantic analysis failed with %d errors\n", context->error_count);
        return false;
    }
}

// Analyze program (top level)
bool semantic_analyze_program(SemanticContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_PROGRAM) return false;
    
    bool success = true;
    
    // First pass: collect all function declarations
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_FUNCTION) {
            // Add function to global scope
            Symbol* func_symbol = symbol_create(child->value, SYMBOL_FUNCTION, child, NULL);
            if (!symbol_table_add_symbol(context->symbol_table, func_symbol)) {
                semantic_add_error(context, SEMANTIC_ERROR_REDEFINED_SYMBOL, 
                                 SEMANTIC_SEVERITY_ERROR, child->line, child->column,
                                 "Function '%s' already defined", child->value);
                symbol_destroy(func_symbol);
                success = false;
            }
        }
    }
    
    // Second pass: analyze function bodies
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_FUNCTION) {
            if (!semantic_analyze_function(context, child)) {
                success = false;
            }
        } else if (child->type == AST_PREPROCESSOR) {
            // Skip preprocessor directives for now
            continue;
        }
    }
    
    return success;
}

// Analyze function
bool semantic_analyze_function(SemanticContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_FUNCTION) return false;
    
    context->current_function = node;
    
    // Enter function scope
    symbol_table_enter_scope(context->symbol_table, true);
    
    bool success = true;
    
    // Analyze parameters
    if (node->child_count > 0 && node->children[0]->type == AST_PARAMETER) {
        ASTNode* params = node->children[0];
        for (int i = 0; i < params->child_count; i++) {
            ASTNode* param = params->children[i];
            if (param->type == AST_PARAMETER) {
                // Add parameter to function scope
                ASTNode* param_type = param->child_count > 0 ? param->children[0] : NULL;
                Symbol* param_symbol = symbol_create(param->value, SYMBOL_PARAMETER, param, param_type);
                param_symbol->is_initialized = true; // Parameters are always initialized
                
                if (!symbol_table_add_symbol(context->symbol_table, param_symbol)) {
                    semantic_add_error(context, SEMANTIC_ERROR_REDEFINED_SYMBOL,
                                     SEMANTIC_SEVERITY_ERROR, param->line, param->column,
                                     "Parameter '%s' already defined", param->value);
                    symbol_destroy(param_symbol);
                    success = false;
                }
            }
        }
    }
    
    // Analyze function body
    ASTNode* body = NULL;
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_BLOCK) {
            body = node->children[i];
            break;
        }
    }
    
    if (body) {
        if (!semantic_analyze_block(context, body)) {
            success = false;
        }
        
        // Check if function has return statement (if not void)
        ASTNode* return_type = NULL;
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type == AST_TYPE) {
                return_type = node->children[i];
                break;
            }
        }
        
        if (return_type && return_type->value && strcmp(return_type->value, "void") != 0) {
            // TODO: Check if all paths return a value
            // For now, just check if there's at least one return statement
            bool has_return = false;
            for (int i = 0; i < body->child_count; i++) {
                if (body->children[i]->type == AST_RETURN) {
                    has_return = true;
                    break;
                }
            }
            
            if (!has_return) {
                semantic_add_error(context, SEMANTIC_ERROR_MISSING_RETURN,
                                 SEMANTIC_SEVERITY_WARNING, node->line, node->column,
                                 "Function '%s' may not return a value on all paths", node->value);
            }
        }
    }
    
    // Exit function scope
    symbol_table_exit_scope(context->symbol_table);
    context->current_function = NULL;
    
    return success;
}

// Analyze block
bool semantic_analyze_block(SemanticContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_BLOCK) return false;
    
    // Enter block scope
    symbol_table_enter_scope(context->symbol_table, false);
    
    bool success = true;
    
    for (int i = 0; i < node->child_count; i++) {
        if (!semantic_analyze_statement(context, node->children[i])) {
            success = false;
        }
    }
    
    // Exit block scope
    symbol_table_exit_scope(context->symbol_table);
    
    return success;
}

// Analyze statement
bool semantic_analyze_statement(SemanticContext* context, ASTNode* node) {
    if (!context || !node) return false;
    
    switch (node->type) {
        case AST_VARIABLE_DECL:
            return semantic_analyze_variable_decl(context, node);
            
        case AST_EXPRESSION_STMT:
            if (node->child_count > 0) {
                return semantic_analyze_expression(context, node->children[0]);
            }
            return true;
            
        case AST_RETURN:
            if (node->child_count > 0) {
                return semantic_analyze_expression(context, node->children[0]);
            }
            return true;
            
        case AST_IF:
        case AST_FOR:
        case AST_WHILE:
            // Analyze condition and body
            for (int i = 0; i < node->child_count; i++) {
                if (!semantic_analyze_statement(context, node->children[i])) {
                    return false;
                }
            }
            return true;
            
        case AST_BLOCK:
            return semantic_analyze_block(context, node);
            
        default:
            return semantic_analyze_expression(context, node);
    }
}

// Analyze variable declaration
bool semantic_analyze_variable_decl(SemanticContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_VARIABLE_DECL) return false;
    
    // Get type
    ASTNode* type_node = node->child_count > 0 ? node->children[0] : NULL;
    if (!type_node) {
        semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                         SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                         "Variable '%s' has no type", node->value);
        return false;
    }
    
    // Create symbol
    Symbol* var_symbol = symbol_create(node->value, SYMBOL_VARIABLE, node, type_node);
    
    // Check for initialization
    if (node->child_count > 1) {
        var_symbol->is_initialized = true;
        // Analyze initialization expression
        if (!semantic_analyze_expression(context, node->children[1])) {
            symbol_destroy(var_symbol);
            return false;
        }
    }
    
    // Add to current scope
    if (!symbol_table_add_symbol(context->symbol_table, var_symbol)) {
        semantic_add_error(context, SEMANTIC_ERROR_REDEFINED_SYMBOL,
                         SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                         "Variable '%s' already defined in this scope", node->value);
        symbol_destroy(var_symbol);
        return false;
    }
    
    return true;
}

// Analyze expression
bool semantic_analyze_expression(SemanticContext* context, ASTNode* node) {
    if (!context || !node) return false;
    
    switch (node->type) {
        case AST_IDENTIFIER: {
            // Check if identifier is defined
            Symbol* symbol = symbol_table_lookup(context->symbol_table, node->value);
            if (!symbol) {
                semantic_add_error(context, SEMANTIC_ERROR_UNDEFINED_SYMBOL,
                                 SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                                 "Undefined symbol '%s'", node->value);
                return false;
            }
            
            // Check if variable is initialized
            if (symbol->type == SYMBOL_VARIABLE && !symbol->is_initialized) {
                semantic_add_error(context, SEMANTIC_ERROR_UNINITIALIZED_VARIABLE,
                                 SEMANTIC_SEVERITY_WARNING, node->line, node->column,
                                 "Variable '%s' used before initialization", node->value);
            }
            
            return true;
        }
        
        case AST_CALL:
            return semantic_validate_function_call(context, node);
            
        case AST_BINARY_OP:
        case AST_UNARY_OP:
        case AST_ASSIGNMENT:
            // Analyze operands
            for (int i = 0; i < node->child_count; i++) {
                if (!semantic_analyze_expression(context, node->children[i])) {
                    return false;
                }
            }
            return true;
            
        case AST_LITERAL:
            return true; // Literals are always valid
            
        default:
            // Recursively analyze children
            for (int i = 0; i < node->child_count; i++) {
                if (!semantic_analyze_expression(context, node->children[i])) {
                    return false;
                }
            }
            return true;
    }
}

// Validate function call
bool semantic_validate_function_call(SemanticContext* context, ASTNode* call) {
    if (!context || !call || call->type != AST_CALL) return false;
    
    if (call->child_count == 0) return false;
    
    ASTNode* callee = call->children[0];
    if (callee->type != AST_IDENTIFIER) {
        // For now, only support simple function calls
        return semantic_analyze_expression(context, callee);
    }
    
    // Look up function
    Symbol* func_symbol = symbol_table_lookup(context->symbol_table, callee->value);
    if (!func_symbol) {
        semantic_add_error(context, SEMANTIC_ERROR_UNDEFINED_FUNCTION,
                         SEMANTIC_SEVERITY_ERROR, call->line, call->column,
                         "Undefined function '%s'", callee->value);
        return false;
    }
    
    if (func_symbol->type != SYMBOL_FUNCTION) {
        semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                         SEMANTIC_SEVERITY_ERROR, call->line, call->column,
                         "'%s' is not a function", callee->value);
        return false;
    }
    
    // Analyze arguments
    bool success = true;
    for (int i = 1; i < call->child_count; i++) {
        if (!semantic_analyze_expression(context, call->children[i])) {
            success = false;
        }
    }
    
    // TODO: Check argument count and types against function signature
    
    return success;
}

// Placeholder implementations for remaining functions
bool semantic_check_types_compatible(ASTNode* type1, ASTNode* type2) {
    if (!type1 || !type2) return false;
    if (!type1->value || !type2->value) return false;
    return strcmp(type1->value, type2->value) == 0;
}

ASTNode* semantic_get_expression_type(SemanticContext* context, ASTNode* expr) {
    // TODO: Implement type inference
    (void)context;
    (void)expr;
    return NULL;
}

bool semantic_validate_assignment(SemanticContext* context, ASTNode* lhs, ASTNode* rhs) {
    // TODO: Implement assignment validation
    (void)context;
    (void)lhs;
    (void)rhs;
    return true;
}

bool semantic_check_return_paths(SemanticContext* context, ASTNode* function) {
    // TODO: Implement return path analysis
    (void)context;
    (void)function;
    return true;
}

bool semantic_check_unreachable_code(SemanticContext* context, ASTNode* block) {
    // TODO: Implement unreachable code detection
    (void)context;
    (void)block;
    return true;
}

bool semantic_check_memory_management(SemanticContext* context, ASTNode* node) {
    // TODO: Implement memory management analysis
    (void)context;
    (void)node;
    return true;
} 