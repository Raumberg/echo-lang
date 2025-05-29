#ifndef TYPE_INFERENCE_H
#define TYPE_INFERENCE_H

#include "../ast/ast.h"
#include "semantic.h"
#include <stdbool.h>

// Forward declarations
typedef struct TypeInferenceContext TypeInferenceContext;
typedef struct GenericInstantiation GenericInstantiation;

// Type constraint for inference
typedef struct TypeConstraint {
    char* variable;     // Type variable name (e.g., "T")
    char* inferred_type; // Inferred concrete type (e.g., "i32")
    ASTNode* context_node; // AST node where constraint was generated
} TypeConstraint;

// Generic function instantiation
struct GenericInstantiation {
    ASTNode* original_function;    // Original generic function
    char** type_arguments;         // Concrete types for each type parameter
    int type_arg_count;            // Number of type arguments
    ASTNode* instantiated_function; // Generated concrete function
    char* mangled_name;            // Unique name for this instantiation
    GenericInstantiation* next;    // Linked list of instantiations
};

// Type inference context
struct TypeInferenceContext {
    TypeConstraint* constraints;     // Array of type constraints
    int constraint_count;            // Number of constraints
    int constraint_capacity;         // Capacity of constraints array
    GenericInstantiation* instantiations; // List of function instantiations
    ASTNode* current_function;       // Currently analyzed function
    bool inference_enabled;          // Whether type inference is active
};

// Main interface functions
TypeInferenceContext* type_inference_create(void);
void type_inference_destroy(TypeInferenceContext* ctx);

// Type inference workflow
bool type_inference_analyze_function(TypeInferenceContext* ctx, ASTNode* function);
bool type_inference_infer_call(TypeInferenceContext* ctx, ASTNode* call_node, ASTNode* target_function, struct SymbolTable* symbol_table);
ASTNode* type_inference_instantiate_generic(TypeInferenceContext* ctx, 
                                           ASTNode* generic_function, 
                                           char** type_args, int type_count);

// Type constraint management
void type_inference_add_constraint(TypeInferenceContext* ctx, const char* variable, 
                                  const char* concrete_type, ASTNode* context);
const char* type_inference_resolve_type(TypeInferenceContext* ctx, const char* type_var);
bool type_inference_unify_types(TypeInferenceContext* ctx, const char* type1, const char* type2);

// Generic instantiation management
GenericInstantiation* type_inference_find_instantiation(TypeInferenceContext* ctx, 
                                                       ASTNode* generic_function,
                                                       char** type_args, int type_count);
char* type_inference_mangle_name(const char* base_name, char** type_args, int type_count);

// Type analysis utilities
char* type_inference_infer_expression_type(TypeInferenceContext* ctx, ASTNode* expr);
char* type_inference_infer_expression_type_with_symbols(TypeInferenceContext* ctx, ASTNode* expr, struct SymbolTable* symbol_table);
bool type_inference_is_concrete_type(const char* type_name);
bool type_inference_types_compatible(const char* type1, const char* type2);

// Debug and utility functions
void type_inference_print_constraints(TypeInferenceContext* ctx);
void type_inference_print_instantiations(TypeInferenceContext* ctx);

#endif // TYPE_INFERENCE_H 