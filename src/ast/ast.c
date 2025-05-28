#define _GNU_SOURCE
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// AST node type names for debugging
const char* ast_node_type_names[] = {
    "PROGRAM", "FUNCTION", "VARIABLE_DECL", "PARAMETER", "BLOCK",
    "RETURN", "IF", "FOR", "WHILE", "BINARY_OP", "UNARY_OP",
    "CALL", "IDENTIFIER", "LITERAL", "TYPE", "STRUCT", "ENUM",
    "ASSIGNMENT", "ARRAY_ACCESS", "MEMBER_ACCESS", "POINTER_DEREF",
    "ADDRESS_OF", "ALLOC", "DELETE", "PREPROCESSOR", "EXPRESSION_STMT",
    "SCOPE_RESOLUTION"
};

// Create a new AST node
ASTNode* ast_create_node(ASTNodeType type, const char* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->line = 0;
    node->column = 0;
    node->data_type = NULL;
    node->is_pointer = false;
    node->is_optional = false;
    node->is_array = false;
    
    return node;
}

// Create binary operation node
ASTNode* ast_create_binary_op(const char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = ast_create_node(AST_BINARY_OP, op);
    if (!node) return NULL;
    
    ast_add_child(node, left);
    ast_add_child(node, right);
    
    return node;
}

// Create unary operation node
ASTNode* ast_create_unary_op(const char* op, ASTNode* operand) {
    ASTNode* node = ast_create_node(AST_UNARY_OP, op);
    if (!node) return NULL;
    
    ast_add_child(node, operand);
    
    return node;
}

// Create function node
ASTNode* ast_create_function(const char* name, ASTNode* params, ASTNode* return_type, ASTNode* body) {
    ASTNode* node = ast_create_node(AST_FUNCTION, name);
    if (!node) return NULL;
    
    if (params) ast_add_child(node, params);
    if (return_type) ast_add_child(node, return_type);
    if (body) ast_add_child(node, body);
    
    return node;
}

// Create literal node
ASTNode* ast_create_literal(const char* value, const char* type) {
    ASTNode* node = ast_create_node(AST_LITERAL, value);
    if (!node) return NULL;
    
    if (type) {
        node->data_type = strdup(type);
    }
    
    return node;
}

// Create identifier node
ASTNode* ast_create_identifier(const char* name) {
    return ast_create_node(AST_IDENTIFIER, name);
}

// Add child to AST node
void ast_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    // Resize children array if needed
    if (parent->child_count >= parent->child_capacity) {
        int new_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        ASTNode** new_children = realloc(parent->children, new_capacity * sizeof(ASTNode*));
        if (!new_children) return;
        
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count++] = child;
}

// Set position information
void ast_set_position(ASTNode* node, int line, int column) {
    if (!node) return;
    node->line = line;
    node->column = column;
}

// Set type information
void ast_set_type_info(ASTNode* node, const char* type, bool is_pointer, bool is_optional) {
    if (!node) return;
    
    if (node->data_type) {
        free(node->data_type);
    }
    node->data_type = type ? strdup(type) : NULL;
    node->is_pointer = is_pointer;
    node->is_optional = is_optional;
}

// Destroy AST node and all children
void ast_destroy(ASTNode* node) {
    if (!node) return;
    
    // Destroy all children
    for (int i = 0; i < node->child_count; i++) {
        ast_destroy(node->children[i]);
    }
    
    // Free memory
    free(node->value);
    free(node->data_type);
    free(node->children);
    free(node);
}

// Print AST for debugging
void ast_print(ASTNode* node, int indent) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    // Print node info
    printf("%s", ast_node_type_names[node->type]);
    if (node->value) {
        printf(" \"%s\"", node->value);
    }
    if (node->data_type) {
        printf(" (%s", node->data_type);
        if (node->is_pointer) printf("*");
        if (node->is_optional) printf("?");
        if (node->is_array) printf("[]");
        printf(")");
    }
    if (node->line > 0) {
        printf(" [%d:%d]", node->line, node->column);
    }
    printf("\n");
    
    // Print children
    for (int i = 0; i < node->child_count; i++) {
        ast_print(node->children[i], indent + 1);
    }
}

// Find function by name in program
ASTNode* ast_find_function(ASTNode* program, const char* name) {
    if (!program || program->type != AST_PROGRAM || !name) {
        return NULL;
    }
    
    for (int i = 0; i < program->child_count; i++) {
        ASTNode* child = program->children[i];
        if (child->type == AST_FUNCTION && child->value && 
            strcmp(child->value, name) == 0) {
            return child;
        }
    }
    
    return NULL;
}

// Get child count
int ast_get_child_count(ASTNode* node) {
    return node ? node->child_count : 0;
}

// Get child by index
ASTNode* ast_get_child(ASTNode* node, int index) {
    if (!node || index < 0 || index >= node->child_count) {
        return NULL;
    }
    return node->children[index];
} 