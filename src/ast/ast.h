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
    AST_SCOPE_RESOLUTION,
    AST_STRUCT_LITERAL,
    // Generics support
    AST_AUTO_TYPE,              // auto keyword
    AST_GENERIC_FUNCTION,       // Generic function with auto parameters
    AST_TEMPLATE_INSTANTIATION, // Instantiated template function
    AST_TYPE_PARAMETER          // Template parameter
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
    
    // Generics support
    bool is_generic;              // Is this a generic function/type?
    bool is_auto;                 // Is this an auto type?
    char** type_parameters;       // List of type parameter names (for generic functions)
    char** inferred_types;        // Resolved types (for instantiated templates)
    int type_param_count;         // Number of type parameters
    ASTNode* generic_template;    // Reference to original generic function (for instantiated functions)
    char* instantiation_key;     // Unique key for this instantiation
};

// AST creation functions
ASTNode* ast_create_node(ASTNodeType type, const char* value);
ASTNode* ast_create_binary_op(const char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_create_unary_op(const char* op, ASTNode* operand);
ASTNode* ast_create_function(const char* name, ASTNode* params, ASTNode* return_type, ASTNode* body);
ASTNode* ast_create_literal(const char* value, const char* type);
ASTNode* ast_create_identifier(const char* name);

// Generics support functions
ASTNode* ast_create_auto_type(void);
ASTNode* ast_create_generic_function(const char* name, ASTNode* type_params, ASTNode* params, 
                                   ASTNode* return_type, ASTNode* body);
ASTNode* ast_create_template_instantiation(ASTNode* generic_func, char** type_args, int type_count);
void ast_mark_as_generic(ASTNode* node);
void ast_add_type_parameter(ASTNode* node, const char* type_name);
void ast_set_inferred_types(ASTNode* node, char** types, int count);

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