#define _GNU_SOURCE
#include "semantic.h"
#include "semantic_errors.h"
#include "import_system.h"
#include "type_inference.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Add builtin modules and functions using new import system
void semantic_add_builtin_modules(SemanticContext* context) {
    if (!context || !context->symbol_table) return;
    
    // Create import context
    ImportContext* import_context = import_context_create(context->symbol_table);
    if (!import_context) return;
    
    // Store import context in semantic context for later use
    context->import_context = import_context;
    
    printf("✓ Import system initialized\n");
}

// Create semantic context
SemanticContext* semantic_create(void) {
    SemanticContext* context = malloc(sizeof(SemanticContext));
    if (!context) return NULL;
    
    context->symbol_table = symbol_table_create();
    if (!context->symbol_table) {
        free(context);
        return NULL;
    }
    
    // Initialize type inference system
    context->type_inference = type_inference_create();
    if (!context->type_inference) {
        symbol_table_destroy(context->symbol_table);
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
    
    // Destroy import context
    if (context->import_context) {
        import_context_destroy(context->import_context);
    }
    
    // Destroy type inference context
    if (context->type_inference) {
        type_inference_destroy(context->type_inference);
    }
    
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
        
        // Print type inference statistics
        if (context->type_inference) {
            type_inference_print_instantiations(context->type_inference);
        }
        
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
    
    // First pass: process #include directives
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_PREPROCESSOR) {
            if (context->import_context) {
                if (!import_process_include(context->import_context, child)) {
                    printf("Failed to process include: %s\n", child->value);
                    success = false;
                }
            }
        }
    }
    
    // Second pass: collect and analyze struct declarations
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_STRUCT) {
            // Analyze struct first
            if (!semantic_analyze_struct(context, child)) {
                success = false;
            }
            
            // Add struct to global scope as a type
            Symbol* struct_symbol = symbol_create(child->value, SYMBOL_STRUCT, child, NULL);
            if (!symbol_table_add_symbol(context->symbol_table, struct_symbol)) {
                semantic_add_error(context, SEMANTIC_ERROR_REDEFINED_SYMBOL, 
                                 SEMANTIC_SEVERITY_ERROR, child->line, child->column,
                                 "Struct '%s' already defined", child->value);
                symbol_destroy(struct_symbol);
                success = false;
            }
        }
    }
    
    // Third pass: collect function declarations
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_FUNCTION || child->type == AST_GENERIC_FUNCTION) {
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
    
    // Fourth pass: analyze function bodies
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_FUNCTION || child->type == AST_GENERIC_FUNCTION) {
            if (!semantic_analyze_function(context, child)) {
                success = false;
            }
        }
    }
    
    return success;
}

// Analyze struct declaration
bool semantic_analyze_struct(SemanticContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_STRUCT) return false;
    
    bool success = true;
    
    printf("✓ Analyzing struct: %s\n", node->value);
    
    // Check all fields in the struct
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* field = node->children[i];
        if (field->type == AST_VARIABLE_DECL) {
            // Check if field type is valid
            if (field->child_count > 0) {
                ASTNode* field_type = field->children[0];
                
                // Rule 1: auto is NOT allowed in struct fields
                if (field_type->type == AST_AUTO_TYPE) {
                    semantic_add_error(context, SEMANTIC_ERROR_INVALID_AUTO_USAGE,
                                     SEMANTIC_SEVERITY_ERROR, field->line, field->column,
                                     "Auto type is not allowed in struct field '%s'. "
                                     "Struct fields must have concrete types", field->value);
                    success = false;
                    continue;
                }
                
                // Rule 2: Check if field type is a valid concrete type
                if (field_type->value) {
                    // Check built-in types
                    bool is_valid_type = (
                        strcmp(field_type->value, "i8") == 0 ||
                        strcmp(field_type->value, "i16") == 0 ||
                        strcmp(field_type->value, "i32") == 0 ||
                        strcmp(field_type->value, "i64") == 0 ||
                        strcmp(field_type->value, "f32") == 0 ||
                        strcmp(field_type->value, "f64") == 0 ||
                        strcmp(field_type->value, "bool") == 0 ||
                        strcmp(field_type->value, "string") == 0 ||
                        strcmp(field_type->value, "char") == 0
                    );
                    
                    // TODO: Also check user-defined types (other structs)
                    // For now, assume any non-builtin type might be valid
                    
                    if (!is_valid_type) {
                        // For now, just warn about unknown types
                        semantic_add_error(context, SEMANTIC_ERROR_UNDEFINED_TYPE,
                                         SEMANTIC_SEVERITY_WARNING, field->line, field->column,
                                         "Unknown type '%s' for field '%s'", 
                                         field_type->value, field->value);
                    }
                } else {
                    semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                                     SEMANTIC_SEVERITY_ERROR, field->line, field->column,
                                     "Field '%s' has no type", field->value);
                    success = false;
                }
            }
        }
    }
    
    return success;
}

// Analyze function
bool semantic_analyze_function(SemanticContext* context, ASTNode* node) {
    if (!context || !node || (node->type != AST_FUNCTION && node->type != AST_GENERIC_FUNCTION)) return false;
    
    context->current_function = node;
    
    // Handle generic functions
    if (node->type == AST_GENERIC_FUNCTION) {
        printf("✓ Analyzing generic function: %s\n", node->value);
        
        // Analyze generic function for type inference
        if (!type_inference_analyze_function(context->type_inference, node)) {
            printf("Failed to analyze generic function for type inference\n");
            return false;
        }
        
        // For generic functions, we don't analyze the body until instantiation
        // Just validate the structure
        return true;
    }
    
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
    
    // Handle auto type inference
    if (type_node->type == AST_AUTO_TYPE && node->child_count > 1) {
        // Variable has auto type and initializer - infer the type
        ASTNode* initializer = node->children[1];
        
        // IMPORTANT: Analyze expression FIRST to create generic instantiations
        if (!semantic_analyze_expression(context, initializer)) {
            return false;
        }
        
        // Now use type inference to determine the actual type
        if (context->type_inference) {
            char* inferred_type = type_inference_infer_expression_type_with_symbols(
                context->type_inference, initializer, context->symbol_table);
            if (inferred_type) {
                printf("✓ Inferred type '%s' for variable '%s'\n", inferred_type, node->value);
                
                // Create a new concrete type node
                ASTNode* concrete_type = ast_create_node(AST_TYPE, inferred_type);
                concrete_type->line = type_node->line;
                concrete_type->column = type_node->column;
                
                // Replace the auto type with the concrete type
                node->children[0] = concrete_type;
                ast_destroy(type_node);
                type_node = concrete_type;
                
                free(inferred_type);
            } else {
                semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                                 SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                                 "Could not infer type for auto variable '%s'", node->value);
                return false;
            }
        } else {
            semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                             SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                             "Type inference not available for auto variable '%s'", node->value);
            return false;
        }
    } else if (type_node->type == AST_AUTO_TYPE) {
        semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                         SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                         "Auto variable '%s' must have an initializer", node->value);
        return false;
    }
    
    // Create symbol
    Symbol* var_symbol = symbol_create(node->value, SYMBOL_VARIABLE, node, type_node);
    
    // Check for initialization
    if (node->child_count > 1) {
        var_symbol->is_initialized = true;
        // Analyze initialization expression (if not already done for auto inference)
        if (type_node->type != AST_AUTO_TYPE) {
            if (!semantic_analyze_expression(context, node->children[1])) {
                symbol_destroy(var_symbol);
                return false;
            }
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
        
        case AST_SCOPE_RESOLUTION: {
            // Handle module::function syntax
            if (node->child_count >= 2) {
                // Build full qualified name (module::submodule::function)
                char full_name[256] = "";
                for (int i = 0; i < node->child_count; i++) {
                    if (i > 0) strcat(full_name, "::");
                    if (node->children[i]->value) {
                        strcat(full_name, node->children[i]->value);
                    }
                }
                
                // Look up the fully qualified symbol
                Symbol* symbol = symbol_table_lookup(context->symbol_table, full_name);
                if (!symbol) {
                    semantic_add_error(context, SEMANTIC_ERROR_UNDEFINED_SYMBOL,
                                     SEMANTIC_SEVERITY_ERROR, node->line, node->column,
                                     "Undefined symbol '%s'", full_name);
                    return false;
                }
                
                return true;
            }
            return false;
        }
        
        case AST_MEMBER_ACCESS: {
            // Handle obj.field and ptr->field syntax with full validation
            return semantic_validate_member_access(context, node);
        }
        
        case AST_STRUCT_LITERAL: {
            // Handle struct initialization syntax: {field: value, ...}
            return semantic_validate_struct_literal(context, node);
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
    
    // For now, just analyze the callee expression
    // The import system will have already added the correct symbols
    if (!semantic_analyze_expression(context, callee)) {
        return false;
    }
    
    // Look up the function being called
    Symbol* func_symbol = NULL;
    if (callee->type == AST_IDENTIFIER) {
        func_symbol = symbol_table_lookup(context->symbol_table, callee->value);
    } else if (callee->type == AST_SCOPE_RESOLUTION) {
        // Handle qualified function calls like io::print
        // For now, just look up by the full qualified name
        if (callee->child_count >= 2) {
            ASTNode* func_name = callee->children[callee->child_count - 1];
            if (func_name->type == AST_IDENTIFIER) {
                func_symbol = symbol_table_lookup(context->symbol_table, func_name->value);
            }
        }
    }
    
    // Analyze arguments
    bool success = true;
    for (int i = 1; i < call->child_count; i++) {
        if (!semantic_analyze_expression(context, call->children[i])) {
            success = false;
        }
    }
    
    // Handle generic function calls
    if (func_symbol && func_symbol->ast_node && 
        func_symbol->ast_node->type == AST_GENERIC_FUNCTION) {
        
        printf("✓ Found call to generic function: %s\n", func_symbol->name);
        
        // Perform type inference for this call
        if (!type_inference_infer_call(context->type_inference, call, func_symbol->ast_node, context->symbol_table)) {
            semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                             SEMANTIC_SEVERITY_ERROR, call->line, call->column,
                             "Failed to infer types for generic function call '%s'", 
                             func_symbol->name);
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
    if (!context || !expr) return NULL;
    
    switch (expr->type) {
        case AST_IDENTIFIER: {
            // Look up symbol and return its type
            Symbol* symbol = symbol_table_lookup(context->symbol_table, expr->value);
            if (symbol && symbol->type_node) {
                return symbol->type_node;
            }
            return NULL;
        }
        
        case AST_LITERAL: {
            // Create type node based on literal type
            if (expr->data_type) {
                ASTNode* type_node = malloc(sizeof(ASTNode));
                if (type_node) {
                    type_node->type = AST_TYPE;
                    type_node->value = strdup(expr->data_type);
                    type_node->child_count = 0;
                    type_node->children = NULL;
                    type_node->line = expr->line;
                    type_node->column = expr->column;
                    type_node->data_type = NULL;
                    return type_node;
                }
            }
            return NULL;
        }
        
        case AST_MEMBER_ACCESS: {
            // Get the type of obj.field
            if (expr->child_count < 2) return NULL;
            
            ASTNode* obj_expr = expr->children[0];
            ASTNode* field_expr = expr->children[1];
            
            // Get the type of the object
            ASTNode* obj_type = semantic_get_expression_type(context, obj_expr);
            if (!obj_type || !obj_type->value) return NULL;
            
            // Find the struct declaration
            Symbol* struct_symbol = symbol_table_lookup(context->symbol_table, obj_type->value);
            if (!struct_symbol || struct_symbol->type != SYMBOL_STRUCT) return NULL;
            
            // Get the field type from struct declaration
            return semantic_get_struct_field_type(struct_symbol->declaration, field_expr->value);
        }
        
        default:
            return NULL;
    }
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

// Check if struct has a specific field
bool semantic_struct_has_field(ASTNode* struct_decl, const char* field_name) {
    if (!struct_decl || struct_decl->type != AST_STRUCT || !field_name) {
        return false;
    }
    
    // Check all fields in the struct
    for (int i = 0; i < struct_decl->child_count; i++) {
        ASTNode* field = struct_decl->children[i];
        if (field->type == AST_VARIABLE_DECL && field->value) {
            if (strcmp(field->value, field_name) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

// Get the type of a specific field in a struct
ASTNode* semantic_get_struct_field_type(ASTNode* struct_decl, const char* field_name) {
    if (!struct_decl || struct_decl->type != AST_STRUCT || !field_name) {
        return NULL;
    }
    
    // Find the field and return its type
    for (int i = 0; i < struct_decl->child_count; i++) {
        ASTNode* field = struct_decl->children[i];
        if (field->type == AST_VARIABLE_DECL && field->value) {
            if (strcmp(field->value, field_name) == 0) {
                // Return the type node (first child of variable declaration)
                if (field->child_count > 0) {
                    return field->children[0];
                }
            }
        }
    }
    
    return NULL;
}

// Validate member access expressions
bool semantic_validate_member_access(SemanticContext* context, ASTNode* member_access) {
    if (!context || !member_access || member_access->type != AST_MEMBER_ACCESS) {
        return false;
    }
    
    if (member_access->child_count < 2) {
        semantic_add_error(context, SEMANTIC_ERROR_INVALID_OPERATION,
                         SEMANTIC_SEVERITY_ERROR, member_access->line, member_access->column,
                         "Invalid member access expression");
        return false;
    }
    
    ASTNode* obj_expr = member_access->children[0];
    ASTNode* field_expr = member_access->children[1];
    
    // Analyze the object expression first
    if (!semantic_analyze_expression(context, obj_expr)) {
        return false;
    }
    
    // Get the type of the object
    ASTNode* obj_type = semantic_get_expression_type(context, obj_expr);
    if (!obj_type || !obj_type->value) {
        semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                         SEMANTIC_SEVERITY_ERROR, member_access->line, member_access->column,
                         "Cannot determine type of object in member access");
        return false;
    }
    
    // Find the struct declaration
    Symbol* struct_symbol = symbol_table_lookup(context->symbol_table, obj_type->value);
    if (!struct_symbol || struct_symbol->type != SYMBOL_STRUCT) {
        semantic_add_error(context, SEMANTIC_ERROR_TYPE_MISMATCH,
                         SEMANTIC_SEVERITY_ERROR, member_access->line, member_access->column,
                         "Member access on non-struct type '%s'", obj_type->value);
        return false;
    }
    
    // Check if the field exists in the struct
    if (!semantic_struct_has_field(struct_symbol->declaration, field_expr->value)) {
        semantic_add_error(context, SEMANTIC_ERROR_UNDEFINED_FIELD,
                         SEMANTIC_SEVERITY_ERROR, member_access->line, member_access->column,
                         "Struct '%s' has no field named '%s'", obj_type->value, field_expr->value);
        return false;
    }
    
    return true;
}

// Validate struct literal expressions
bool semantic_validate_struct_literal(SemanticContext* context, ASTNode* struct_literal) {
    if (!context || !struct_literal || struct_literal->type != AST_STRUCT_LITERAL) {
        return false;
    }
    
    // For now, just validate that all field values are valid expressions
    // In a full implementation, we would:
    // 1. Determine the target struct type from context
    // 2. Check that all required fields are initialized
    // 3. Check that no unknown fields are provided
    // 4. Validate field value types match field declarations
    
    bool success = true;
    for (int i = 0; i < struct_literal->child_count; i++) {
        ASTNode* field_init = struct_literal->children[i];
        if (field_init->type == AST_ASSIGNMENT && field_init->child_count >= 2) {
            // Validate the field value expression
            ASTNode* field_value = field_init->children[1];
            if (!semantic_analyze_expression(context, field_value)) {
                success = false;
            }
        }
    }
    
    return success;
} 