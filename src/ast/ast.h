#ifndef AST_H
#define AST_H

#include <stdbool.h>

// AST node types
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VARIABLE_DECL,
    AST_PARAMETER,
    AST_BLOCK,
    AST_RETURN,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_IDENTIFIER,
    AST_LITERAL,
    AST_TYPE,
    AST_STRUCT,
    AST_ENUM,
    AST_ASSIGNMENT,
    AST_ARRAY_ACCESS,
    AST_MEMBER_ACCESS,
    AST_POINTER_DEREF,
    AST_ADDRESS_OF,
    AST_ALLOC,
    AST_DELETE,
    AST_PREPROCESSOR,
    AST_EXPRESSION_STMT,
    AST_SCOPE_RESOLUTION
} ASTNodeType;

// Forward declaration
typedef struct ASTNode ASTNode;

// AST node structure
struct ASTNode {
    ASTNodeType type;
    char* value;
    ASTNode** children;
    int child_count;
    int child_capacity;
    int line;
    int column;
    
    // Type information
    char* data_type;
    bool is_pointer;
    bool is_optional;
    bool is_array;
};

// AST creation functions
ASTNode* ast_create_node(ASTNodeType type, const char* value);
ASTNode* ast_create_binary_op(const char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_create_unary_op(const char* op, ASTNode* operand);
ASTNode* ast_create_function(const char* name, ASTNode* params, ASTNode* return_type, ASTNode* body);
ASTNode* ast_create_literal(const char* value, const char* type);
ASTNode* ast_create_identifier(const char* name);

// AST manipulation functions
void ast_add_child(ASTNode* parent, ASTNode* child);
void ast_set_position(ASTNode* node, int line, int column);
void ast_set_type_info(ASTNode* node, const char* type, bool is_pointer, bool is_optional);

// AST utility functions
void ast_destroy(ASTNode* node);
void ast_print(ASTNode* node, int indent);
ASTNode* ast_find_function(ASTNode* program, const char* name);
int ast_get_child_count(ASTNode* node);
ASTNode* ast_get_child(ASTNode* node, int index);

// AST node type names for debugging
extern const char* ast_node_type_names[];

#endif // AST_H 