#define _GNU_SOURCE
#include "type_inference.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create type inference context
TypeInferenceContext* type_inference_create(void) {
    TypeInferenceContext* ctx = malloc(sizeof(TypeInferenceContext));
    if (!ctx) return NULL;
    
    ctx->constraints = NULL;
    ctx->constraint_count = 0;
    ctx->constraint_capacity = 0;
    ctx->instantiations = NULL;
    ctx->current_function = NULL;
    ctx->inference_enabled = true;
    
    return ctx;
}

// Destroy type inference context
void type_inference_destroy(TypeInferenceContext* ctx) {
    if (!ctx) return;
    
    // Free constraints
    for (int i = 0; i < ctx->constraint_count; i++) {
        free(ctx->constraints[i].variable);
        free(ctx->constraints[i].inferred_type);
    }
    free(ctx->constraints);
    
    // Free instantiations
    GenericInstantiation* inst = ctx->instantiations;
    while (inst) {
        GenericInstantiation* next = inst->next;
        
        for (int i = 0; i < inst->type_arg_count; i++) {
            free(inst->type_arguments[i]);
        }
        free(inst->type_arguments);
        free(inst->mangled_name);
        
        if (inst->instantiated_function) {
            ast_destroy(inst->instantiated_function);
        }
        
        free(inst);
        inst = next;
    }
    
    free(ctx);
}

// Add type constraint
void type_inference_add_constraint(TypeInferenceContext* ctx, const char* variable, 
                                  const char* concrete_type, ASTNode* context) {
    if (!ctx || !variable || !concrete_type) return;
    
    // Check if constraint already exists for this variable
    for (int i = 0; i < ctx->constraint_count; i++) {
        if (strcmp(ctx->constraints[i].variable, variable) == 0) {
            // Update existing constraint if types are compatible
            if (type_inference_types_compatible(ctx->constraints[i].inferred_type, concrete_type)) {
                free(ctx->constraints[i].inferred_type);
                ctx->constraints[i].inferred_type = strdup(concrete_type);
                return;
            } else {
                printf("Type conflict for variable %s: %s vs %s\n", 
                       variable, ctx->constraints[i].inferred_type, concrete_type);
                return;
            }
        }
    }
    
    // Resize constraints array if needed
    if (ctx->constraint_count >= ctx->constraint_capacity) {
        int new_capacity = ctx->constraint_capacity == 0 ? 4 : ctx->constraint_capacity * 2;
        TypeConstraint* new_constraints = realloc(ctx->constraints, 
                                                 new_capacity * sizeof(TypeConstraint));
        if (!new_constraints) return;
        
        ctx->constraints = new_constraints;
        ctx->constraint_capacity = new_capacity;
    }
    
    // Add new constraint
    TypeConstraint* constraint = &ctx->constraints[ctx->constraint_count++];
    constraint->variable = strdup(variable);
    constraint->inferred_type = strdup(concrete_type);
    constraint->context_node = context;
    
    printf("✓ Added type constraint: %s -> %s\n", variable, concrete_type);
}

// Resolve type variable to concrete type
const char* type_inference_resolve_type(TypeInferenceContext* ctx, const char* type_var) {
    if (!ctx || !type_var) return NULL;
    
    // If it's already a concrete type, return as-is
    if (type_inference_is_concrete_type(type_var)) {
        return type_var;
    }
    
    // Look up in constraints
    for (int i = 0; i < ctx->constraint_count; i++) {
        if (strcmp(ctx->constraints[i].variable, type_var) == 0) {
            return ctx->constraints[i].inferred_type;
        }
    }
    
    return NULL; // Unresolved type variable
}

// Check if type is concrete (not a type variable)
bool type_inference_is_concrete_type(const char* type_name) {
    if (!type_name) return false;
    
    // Built-in types
    if (strcmp(type_name, "i8") == 0 || strcmp(type_name, "i16") == 0 ||
        strcmp(type_name, "i32") == 0 || strcmp(type_name, "i64") == 0 ||
        strcmp(type_name, "f32") == 0 || strcmp(type_name, "f64") == 0 ||
        strcmp(type_name, "bool") == 0 || strcmp(type_name, "string") == 0 ||
        strcmp(type_name, "char") == 0 || strcmp(type_name, "void") == 0) {
        return true;
    }
    
    // TODO: Check for user-defined types (structs, enums)
    
    return false;
}

// Check if two types are compatible
bool type_inference_types_compatible(const char* type1, const char* type2) {
    if (!type1 || !type2) return false;
    
    // Exact match
    if (strcmp(type1, type2) == 0) return true;
    
    // TODO: Add more sophisticated compatibility rules
    // For now, only exact matches are compatible
    
    return false;
}

// Infer type of expression with symbol table access
char* type_inference_infer_expression_type_with_symbols(TypeInferenceContext* ctx, ASTNode* expr, struct SymbolTable* symbol_table) {
    if (!ctx || !expr) return NULL;
    
    switch (expr->type) {
        case AST_LITERAL:
            if (expr->data_type) {
                return strdup(expr->data_type);
            }
            // Try to infer from literal value
            if (strstr(expr->value, ".") != NULL) {
                return strdup("f64"); // Default float type
            } else if (strcmp(expr->value, "true") == 0 || strcmp(expr->value, "false") == 0) {
                return strdup("bool");
            } else if (expr->value[0] == '"') {
                return strdup("string");
            } else {
                return strdup("i32"); // Default integer type
            }
            
        case AST_IDENTIFIER: {
            // Look up variable type in symbol table
            if (symbol_table) {
                Symbol* symbol = symbol_table_lookup(symbol_table, expr->value);
                if (symbol && symbol->type_node && symbol->type_node->value) {
                    printf("✓ Found variable '%s' with type '%s' in symbol table\n", 
                           expr->value, symbol->type_node->value);
                    return strdup(symbol->type_node->value);
                } else {
                    printf("⚠️ Variable '%s' not found in symbol table or has no type\n", expr->value);
                }
            } else {
                printf("⚠️ No symbol table provided for variable '%s'\n", expr->value);
            }
            return strdup("i32"); // Fallback
        }
        
        case AST_CALL: {
            // Handle function calls - this is crucial for generic functions!
            if (expr->child_count == 0) return strdup("i32");
            
            ASTNode* callee = expr->children[0];
            if (callee->type == AST_IDENTIFIER) {
                // Look for generic instantiation that matches this call
                GenericInstantiation* inst = ctx->instantiations;
                while (inst) {
                    // Check if this instantiation matches the call
                    if (inst->instantiated_function && 
                        inst->instantiated_function->value &&
                        strstr(inst->instantiated_function->value, callee->value) == inst->instantiated_function->value) {
                        
                        // Get return type from instantiated function
                        if (inst->instantiated_function->child_count > 0) {
                            ASTNode* return_type = NULL;
                            for (int i = 0; i < inst->instantiated_function->child_count; i++) {
                                if (inst->instantiated_function->children[i]->type == AST_TYPE) {
                                    return_type = inst->instantiated_function->children[i];
                                    break;
                                }
                            }
                            if (return_type && return_type->value) {
                                printf("✓ Found return type '%s' for call to %s\n", 
                                       return_type->value, inst->mangled_name);
                                return strdup(return_type->value);
                            }
                        }
                    }
                    inst = inst->next;
                }
                
                // If no instantiation found, try to infer from original generic function
                // This is a fallback - in practice, instantiation should happen first
                return strdup("i32"); // Fallback
            }
            return strdup("i32");
        }
            
        case AST_STRUCT_LITERAL:
            // For struct literals, the type is the struct name stored in the value
            if (expr->value) {
                return strdup(expr->value);
            }
            return strdup("unknown_struct");
            
        case AST_BINARY_OP:
            // For binary operations, infer from operands
            char* left_type = type_inference_infer_expression_type_with_symbols(ctx, expr->children[0], symbol_table);
            char* right_type = type_inference_infer_expression_type_with_symbols(ctx, expr->children[1], symbol_table);
            
            if (left_type && right_type && strcmp(left_type, right_type) == 0) {
                free(right_type);
                return left_type;
            }
            
            free(left_type);
            free(right_type);
            return strdup("i32"); // Default fallback
            
        default:
            return strdup("i32"); // Default fallback
    }
}

// Infer type of expression
char* type_inference_infer_expression_type(TypeInferenceContext* ctx, ASTNode* expr) {
    if (!ctx || !expr) return NULL;
    
    switch (expr->type) {
        case AST_LITERAL:
            if (expr->data_type) {
                return strdup(expr->data_type);
            }
            // Try to infer from literal value
            if (strstr(expr->value, ".") != NULL) {
                return strdup("f64"); // Default float type
            } else if (strcmp(expr->value, "true") == 0 || strcmp(expr->value, "false") == 0) {
                return strdup("bool");
            } else if (expr->value[0] == '"') {
                return strdup("string");
            } else {
                return strdup("i32"); // Default integer type
            }
            
        case AST_IDENTIFIER:
            // TODO: Look up variable type in symbol table
            // For now, return placeholder
            return strdup("i32"); // Placeholder
            
        case AST_CALL: {
            // Handle function calls - this is crucial for generic functions!
            if (expr->child_count == 0) return strdup("i32");
            
            ASTNode* callee = expr->children[0];
            if (callee->type == AST_IDENTIFIER) {
                // Look for generic instantiation that matches this call
                GenericInstantiation* inst = ctx->instantiations;
                while (inst) {
                    // Check if this instantiation matches the call
                    if (inst->instantiated_function && 
                        inst->instantiated_function->value &&
                        strstr(inst->instantiated_function->value, callee->value) == inst->instantiated_function->value) {
                        
                        // Get return type from instantiated function
                        if (inst->instantiated_function->child_count > 1) {
                            ASTNode* return_type = NULL;
                            for (int i = 0; i < inst->instantiated_function->child_count; i++) {
                                if (inst->instantiated_function->children[i]->type == AST_TYPE) {
                                    return_type = inst->instantiated_function->children[i];
                                    break;
                                }
                            }
                            if (return_type && return_type->value) {
                                return strdup(return_type->value);
                            }
                        }
                    }
                    inst = inst->next;
                }
                
                // If no instantiation found, try to infer from original generic function
                // This is a fallback - in practice, instantiation should happen first
                return strdup("i32"); // Fallback
            }
            return strdup("i32");
        }
            
        case AST_STRUCT_LITERAL:
            // For struct literals, the type is the struct name stored in the value
            if (expr->value) {
                return strdup(expr->value);
            }
            return strdup("unknown_struct");
            
        case AST_BINARY_OP:
            // For binary operations, infer from operands
            char* left_type = type_inference_infer_expression_type(ctx, expr->children[0]);
            char* right_type = type_inference_infer_expression_type(ctx, expr->children[1]);
            
            if (left_type && right_type && strcmp(left_type, right_type) == 0) {
                free(right_type);
                return left_type;
            }
            
            free(left_type);
            free(right_type);
            return strdup("i32"); // Default fallback
            
        default:
            return strdup("i32"); // Default fallback
    }
}

// Analyze generic function for type inference
bool type_inference_analyze_function(TypeInferenceContext* ctx, ASTNode* function) {
    if (!ctx || !function) return false;
    
    if (function->type != AST_GENERIC_FUNCTION) {
        return true; // Not a generic function, nothing to analyze
    }
    
    ctx->current_function = function;
    
    printf("✓ Analyzing generic function: %s\n", function->value);
    
    // For generic functions, we collect type parameters but don't instantiate yet
    // Instantiation happens when the function is called
    
    return true;
}

// Infer types for function call and potentially instantiate generic
bool type_inference_infer_call(TypeInferenceContext* ctx, ASTNode* call_node, 
                              ASTNode* target_function, struct SymbolTable* symbol_table) {
    if (!ctx || !call_node || !target_function) return false;
    
    if (target_function->type != AST_GENERIC_FUNCTION) {
        return true; // Not a generic function call
    }
    
    printf("✓ Inferring types for call to generic function: %s\n", target_function->value);
    
    // Get function parameters (first child after function name)
    ASTNode* params = NULL;
    for (int i = 0; i < target_function->child_count; i++) {
        if (target_function->children[i]->type == AST_PARAMETER) {
            params = target_function->children[i];
            break;
        }
    }
    
    if (!params) return false;
    
    // Collect type arguments from call arguments
    int arg_count = call_node->child_count - 1; // First child is function name
    if (arg_count != params->child_count) {
        printf("Argument count mismatch in generic call\n");
        return false;
    }
    
    char** type_args = malloc(arg_count * sizeof(char*));
    if (!type_args) return false;
    
    for (int i = 0; i < arg_count; i++) {
        ASTNode* arg = call_node->children[i + 1];
        type_args[i] = type_inference_infer_expression_type_with_symbols(ctx, arg, symbol_table);
        if (!type_args[i]) {
            // Free allocated types on error
            for (int j = 0; j < i; j++) {
                free(type_args[j]);
            }
            free(type_args);
            return false;
        }
    }
    
    // Find or create instantiation
    GenericInstantiation* inst = type_inference_find_instantiation(ctx, target_function, 
                                                                   type_args, arg_count);
    if (!inst) {
        // Create new instantiation
        ASTNode* instantiated = type_inference_instantiate_generic(ctx, target_function, 
                                                                  type_args, arg_count);
        if (instantiated) {
            printf("✓ Created new instantiation: %s\n", instantiated->value);
        }
    }
    
    // Free type arguments
    for (int i = 0; i < arg_count; i++) {
        free(type_args[i]);
    }
    free(type_args);
    
    return true;
}

// Find existing instantiation
GenericInstantiation* type_inference_find_instantiation(TypeInferenceContext* ctx, 
                                                       ASTNode* generic_function,
                                                       char** type_args, int type_count) {
    if (!ctx) return NULL;
    
    GenericInstantiation* inst = ctx->instantiations;
    while (inst) {
        if (inst->original_function == generic_function && 
            inst->type_arg_count == type_count) {
            
            // Check if type arguments match
            bool match = true;
            for (int i = 0; i < type_count; i++) {
                if (strcmp(inst->type_arguments[i], type_args[i]) != 0) {
                    match = false;
                    break;
                }
            }
            
            if (match) {
                return inst;
            }
        }
        inst = inst->next;
    }
    
    return NULL;
}

// Generate mangled name for instantiation
char* type_inference_mangle_name(const char* base_name, char** type_args, int type_count) {
    if (!base_name) return NULL;
    
    // Calculate required size
    size_t size = strlen(base_name) + 1; // base name + null terminator
    size += 1; // underscore
    
    for (int i = 0; i < type_count; i++) {
        if (type_args[i]) {
            size += strlen(type_args[i]) + 1; // type + underscore
        }
    }
    
    char* mangled = malloc(size);
    if (!mangled) return NULL;
    
    strcpy(mangled, base_name);
    strcat(mangled, "_");
    
    for (int i = 0; i < type_count; i++) {
        if (type_args[i]) {
            strcat(mangled, type_args[i]);
            if (i < type_count - 1) {
                strcat(mangled, "_");
            }
        }
    }
    
    return mangled;
}

// Instantiate generic function with concrete types
ASTNode* type_inference_instantiate_generic(TypeInferenceContext* ctx, 
                                           ASTNode* generic_function, 
                                           char** type_args, int type_count) {
    if (!ctx || !generic_function || !type_args) return NULL;
    
    // Create new instantiation entry
    GenericInstantiation* inst = malloc(sizeof(GenericInstantiation));
    if (!inst) return NULL;
    
    inst->original_function = generic_function;
    inst->type_arg_count = type_count;
    inst->type_arguments = malloc(type_count * sizeof(char*));
    
    if (!inst->type_arguments) {
        free(inst);
        return NULL;
    }
    
    for (int i = 0; i < type_count; i++) {
        inst->type_arguments[i] = strdup(type_args[i]);
    }
    
    inst->mangled_name = type_inference_mangle_name(generic_function->value, 
                                                   type_args, type_count);
    
    // Create proper instantiated function AST with correct return type
    inst->instantiated_function = ast_create_node(AST_FUNCTION, inst->mangled_name);
    
    // Find return type from original function
    ASTNode* original_return_type = NULL;
    for (int i = 0; i < generic_function->child_count; i++) {
        if (generic_function->children[i]->type == AST_TYPE || 
            generic_function->children[i]->type == AST_AUTO_TYPE) {
            original_return_type = generic_function->children[i];
            break;
        }
    }
    
    // Create concrete return type
    if (original_return_type) {
        ASTNode* concrete_return_type = NULL;
        
        if (original_return_type->type == AST_AUTO_TYPE) {
            // For auto return type, infer from the first parameter type
            // This is a simple heuristic - in practice, we'd need more sophisticated analysis
            if (type_count > 0) {
                concrete_return_type = ast_create_node(AST_TYPE, type_args[0]);
            } else {
                concrete_return_type = ast_create_node(AST_TYPE, "i32");
            }
        } else {
            // Copy concrete return type
            concrete_return_type = ast_create_node(AST_TYPE, original_return_type->value);
        }
        
        if (concrete_return_type) {
            ast_add_child(inst->instantiated_function, concrete_return_type);
        }
    }
    
    // Add to instantiation list
    inst->next = ctx->instantiations;
    ctx->instantiations = inst;
    
    printf("✓ Instantiated %s with types: ", generic_function->value);
    for (int i = 0; i < type_count; i++) {
        printf("%s", type_args[i]);
        if (i < type_count - 1) printf(", ");
    }
    printf(" -> %s", inst->mangled_name);
    
    // Print return type info
    if (inst->instantiated_function->child_count > 0) {
        ASTNode* ret_type = inst->instantiated_function->children[0];
        if (ret_type && ret_type->value) {
            printf(" (returns %s)", ret_type->value);
        }
    }
    printf("\n");
    
    return inst->instantiated_function;
}

// Debug: Print type constraints
void type_inference_print_constraints(TypeInferenceContext* ctx) {
    if (!ctx) return;
    
    printf("\n=== Type Constraints ===\n");
    for (int i = 0; i < ctx->constraint_count; i++) {
        printf("%s -> %s\n", ctx->constraints[i].variable, ctx->constraints[i].inferred_type);
    }
    printf("========================\n\n");
}

// Debug: Print instantiations
void type_inference_print_instantiations(TypeInferenceContext* ctx) {
    if (!ctx) return;
    
    printf("\n=== Generic Instantiations ===\n");
    GenericInstantiation* inst = ctx->instantiations;
    while (inst) {
        printf("%s(", inst->original_function->value);
        for (int i = 0; i < inst->type_arg_count; i++) {
            printf("%s", inst->type_arguments[i]);
            if (i < inst->type_arg_count - 1) printf(", ");
        }
        printf(") -> %s\n", inst->mangled_name);
        inst = inst->next;
    }
    printf("==============================\n\n");
} 