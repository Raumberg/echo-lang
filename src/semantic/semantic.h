#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "symbol_table.h"
#include "semantic_errors.h"
#include <stdbool.h>

// Semantic context structure
typedef struct SemanticContext {
    SymbolTable* symbol_table;
    ASTNode* current_function;     // Currently analyzed function
    SemanticError* errors;         // Linked list of errors
    int error_count;
    int warning_count;
    bool has_fatal_error;
    char* current_filename;
} SemanticContext;

// Main semantic analysis functions
SemanticContext* semantic_create(void);
void semantic_destroy(SemanticContext* context);
bool semantic_analyze(SemanticContext* context, ASTNode* ast);

// Error handling
void semantic_add_error(SemanticContext* context, SemanticErrorType type, 
                       SemanticSeverity severity, int line, int column, 
                       const char* format, ...);
void semantic_print_errors(SemanticContext* context);
bool semantic_has_errors(SemanticContext* context);

// AST traversal functions
bool semantic_analyze_program(SemanticContext* context, ASTNode* node);
bool semantic_analyze_function(SemanticContext* context, ASTNode* node);
bool semantic_analyze_variable_decl(SemanticContext* context, ASTNode* node);
bool semantic_analyze_statement(SemanticContext* context, ASTNode* node);
bool semantic_analyze_expression(SemanticContext* context, ASTNode* node);
bool semantic_analyze_block(SemanticContext* context, ASTNode* node);

// Type checking
bool semantic_check_types_compatible(ASTNode* type1, ASTNode* type2);
ASTNode* semantic_get_expression_type(SemanticContext* context, ASTNode* expr);
bool semantic_validate_assignment(SemanticContext* context, ASTNode* lhs, ASTNode* rhs);
bool semantic_validate_function_call(SemanticContext* context, ASTNode* call);

// Control flow analysis
bool semantic_check_return_paths(SemanticContext* context, ASTNode* function);
bool semantic_check_unreachable_code(SemanticContext* context, ASTNode* block);

// Memory management analysis
bool semantic_check_memory_management(SemanticContext* context, ASTNode* node);

#endif // SEMANTIC_H 