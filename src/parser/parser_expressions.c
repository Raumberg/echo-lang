#define _GNU_SOURCE
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Parse expression (entry point)
ASTNode* parse_expression(Parser* parser) {
    return parse_assignment(parser);
}

// Parse assignment (lowest precedence)
ASTNode* parse_assignment(Parser* parser) {
    ASTNode* expr = parse_logical_or(parser);
    if (!expr) return NULL;
    
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && strcmp(parser->current_token.value, "=") == 0) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_assignment(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        ASTNode* assignment = ast_create_node(AST_ASSIGNMENT, op);
        ast_add_child(assignment, expr);
        ast_add_child(assignment, right);
        free(op);
        
        return assignment;
    }
    
    return expr;
}

// Parse logical OR
ASTNode* parse_logical_or(Parser* parser) {
    ASTNode* expr = parse_logical_and(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && strcmp(parser->current_token.value, "||") == 0) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_logical_and(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse logical AND
ASTNode* parse_logical_and(Parser* parser) {
    ASTNode* expr = parse_equality(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && strcmp(parser->current_token.value, "&&") == 0) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_equality(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse equality (== !=)
ASTNode* parse_equality(Parser* parser) {
    ASTNode* expr = parse_comparison(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && 
           (strcmp(parser->current_token.value, "==") == 0 || 
            strcmp(parser->current_token.value, "!=") == 0)) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_comparison(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse comparison (< > <= >=)
ASTNode* parse_comparison(Parser* parser) {
    ASTNode* expr = parse_term(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && 
           (strcmp(parser->current_token.value, "<") == 0 || 
            strcmp(parser->current_token.value, ">") == 0 ||
            strcmp(parser->current_token.value, "<=") == 0 || 
            strcmp(parser->current_token.value, ">=") == 0)) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_term(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse term (+ -)
ASTNode* parse_term(Parser* parser) {
    ASTNode* expr = parse_factor(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && 
           (strcmp(parser->current_token.value, "+") == 0 || 
            strcmp(parser->current_token.value, "-") == 0)) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_factor(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse factor (* / %)
ASTNode* parse_factor(Parser* parser) {
    ASTNode* expr = parse_unary(parser);
    if (!expr) return NULL;
    
    while (parser_check(parser, TOKEN_OPERATOR) && 
           parser->current_token.value && 
           (strcmp(parser->current_token.value, "*") == 0 || 
            strcmp(parser->current_token.value, "/") == 0 ||
            strcmp(parser->current_token.value, "%") == 0)) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* right = parse_unary(parser);
        if (!right) {
            free(op);
            ast_destroy(expr);
            return NULL;
        }
        
        expr = ast_create_binary_op(op, expr, right);
        free(op);
    }
    
    return expr;
}

// Parse unary (! - + * & ++ --)
ASTNode* parse_unary(Parser* parser) {
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && is_unary_operator(parser->current_token.value)) {
        
        char* op = strdup(parser->current_token.value);
        parser_advance(parser);
        
        ASTNode* operand = parse_unary(parser);
        if (!operand) {
            free(op);
            return NULL;
        }
        
        ASTNode* unary = ast_create_unary_op(op, operand);
        free(op);
        return unary;
    }
    
    // Handle alloc keyword
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && strcmp(parser->current_token.value, "alloc") == 0) {
        
        ASTNode* alloc_node = ast_create_node(AST_ALLOC, "alloc");
        ast_set_position(alloc_node, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        
        // Parse type
        ASTNode* type_node = parse_type(parser);
        if (!type_node) {
            ast_destroy(alloc_node);
            return NULL;
        }
        ast_add_child(alloc_node, type_node);
        
        // Optional initialization
        if (parser_check(parser, TOKEN_DELIMITER) && 
            parser->current_token.value && strcmp(parser->current_token.value, "(") == 0) {
            parser_advance(parser);
            
            ASTNode* init_expr = parse_expression(parser);
            if (init_expr) {
                ast_add_child(alloc_node, init_expr);
            }
            
            if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after alloc initialization")) {
                ast_destroy(alloc_node);
                return NULL;
            }
        }
        
        return alloc_node;
    }
    
    // Handle delete keyword
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && strcmp(parser->current_token.value, "delete") == 0) {
        
        ASTNode* delete_node = ast_create_node(AST_DELETE, "delete");
        ast_set_position(delete_node, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        
        ASTNode* operand = parse_unary(parser);
        if (!operand) {
            ast_destroy(delete_node);
            return NULL;
        }
        ast_add_child(delete_node, operand);
        
        return delete_node;
    }
    
    return parse_postfix(parser);
}

// Parse postfix expressions (function calls, member access, scope resolution)
ASTNode* parse_postfix(Parser* parser) {
    ASTNode* expr = parse_primary(parser);
    if (!expr) return NULL;
    
    while (true) {
        // Scope resolution ::
        if (parser_check(parser, TOKEN_OPERATOR) && 
            parser->current_token.value && strcmp(parser->current_token.value, "::") == 0) {
            parser_advance(parser);
            
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected identifier after '::'");
                ast_destroy(expr);
                return NULL;
            }
            
            ASTNode* right = ast_create_identifier(parser->current_token.value);
            ast_set_position(right, parser->current_token.line, parser->current_token.column);
            parser_advance(parser);
            
            ASTNode* scope_res = ast_create_node(AST_SCOPE_RESOLUTION, "::");
            ast_add_child(scope_res, expr);
            ast_add_child(scope_res, right);
            expr = scope_res;
        }
        // Function calls (expr(...))
        else if (parser_check(parser, TOKEN_DELIMITER) && 
            parser->current_token.value && strcmp(parser->current_token.value, "(") == 0) {
            expr = parse_call(parser, expr);
            if (!expr) return NULL;
        }
        // Named struct literals (TypeName {field: value})
        else if (expr->type == AST_IDENTIFIER && 
                 parser_check(parser, TOKEN_DELIMITER) && 
                 parser->current_token.value && strcmp(parser->current_token.value, "{") == 0) {
            parser_advance(parser); // consume '{'
            
            ASTNode* struct_literal = ast_create_node(AST_STRUCT_LITERAL, expr->value);
            ast_set_position(struct_literal, parser->current_token.line, parser->current_token.column);
            
            // Parse field initializers
            if (!parser_check(parser, TOKEN_DELIMITER) || 
                strcmp(parser->current_token.value, "}") != 0) {
                
                do {
                    // Parse field name
                    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                        parser_error(parser, "Expected field name in struct literal");
                        ast_destroy(struct_literal);
                        ast_destroy(expr);
                        return NULL;
                    }
                    
                    ASTNode* field_name = ast_create_identifier(parser->current_token.value);
                    ast_set_position(field_name, parser->current_token.line, parser->current_token.column);
                    parser_advance(parser);
                    
                    // Expect colon
                    if (!parser_check(parser, TOKEN_OPERATOR) || 
                        !parser->current_token.value || strcmp(parser->current_token.value, ":") != 0) {
                        parser_error(parser, "Expected ':' after field name in struct literal");
                        ast_destroy(field_name);
                        ast_destroy(struct_literal);
                        ast_destroy(expr);
                        return NULL;
                    }
                    parser_advance(parser);
                    
                    // Parse field value
                    ASTNode* field_value = parse_expression(parser);
                    if (!field_value) {
                        ast_destroy(field_name);
                        ast_destroy(struct_literal);
                        ast_destroy(expr);
                        return NULL;
                    }
                    
                    // Create field initializer node
                    ASTNode* field_init = ast_create_node(AST_ASSIGNMENT, ":");
                    ast_add_child(field_init, field_name);
                    ast_add_child(field_init, field_value);
                    ast_add_child(struct_literal, field_init);
                    
                    // Check for comma
                    if (parser_check(parser, TOKEN_DELIMITER) && 
                        parser->current_token.value && strcmp(parser->current_token.value, ",") == 0) {
                        parser_advance(parser);
                        continue;
                    } else {
                        break;
                    }
                } while (true);
            }
            
            // Expect closing brace
            if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '}' after struct literal")) {
                ast_destroy(struct_literal);
                ast_destroy(expr);
                return NULL;
            }
            
            ast_destroy(expr); // free the identifier since we consumed it
            expr = struct_literal;
        }
        // Member access with dot (obj.field)
        else if (parser_check(parser, TOKEN_OPERATOR) && 
                 parser->current_token.value && strcmp(parser->current_token.value, ".") == 0) {
            parser_advance(parser);
            
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected field name after '.'");
                ast_destroy(expr);
                return NULL;
            }
            
            ASTNode* field = ast_create_identifier(parser->current_token.value);
            ast_set_position(field, parser->current_token.line, parser->current_token.column);
            parser_advance(parser);
            
            ASTNode* member_access = ast_create_node(AST_MEMBER_ACCESS, ".");
            ast_add_child(member_access, expr);
            ast_add_child(member_access, field);
            expr = member_access;
        }
        // Pointer member access (ptr->field)
        else if (parser_check(parser, TOKEN_OPERATOR) && 
                 parser->current_token.value && strcmp(parser->current_token.value, "->") == 0) {
            parser_advance(parser);
            
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected field name after '->'");
                ast_destroy(expr);
                return NULL;
            }
            
            ASTNode* field = ast_create_identifier(parser->current_token.value);
            ast_set_position(field, parser->current_token.line, parser->current_token.column);
            parser_advance(parser);
            
            ASTNode* member_access = ast_create_node(AST_MEMBER_ACCESS, "->");
            ast_add_child(member_access, expr);
            ast_add_child(member_access, field);
            expr = member_access;
        }
        // Array access [] (TODO: implement later)
        else {
            break;
        }
    }
    
    return expr;
}

// Parse primary expressions
ASTNode* parse_primary(Parser* parser) {
    // Literals
    if (parser_check(parser, TOKEN_INTEGER)) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "integer");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    if (parser_check(parser, TOKEN_FLOAT)) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "float");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    if (parser_check(parser, TOKEN_STRING)) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "string");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    if (parser_check(parser, TOKEN_CHAR)) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "char");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    // Boolean literals
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && 
        (strcmp(parser->current_token.value, "true") == 0 || 
         strcmp(parser->current_token.value, "false") == 0)) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "bool");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    // Null literal
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && strcmp(parser->current_token.value, "null") == 0) {
        ASTNode* literal = ast_create_literal(parser->current_token.value, "null");
        ast_set_position(literal, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        return literal;
    }
    
    // Identifiers
    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        ASTNode* identifier = ast_create_identifier(parser->current_token.value);
        ast_set_position(identifier, parser->current_token.line, parser->current_token.column);
        parser_advance(parser);
        
        return identifier;
    }
    
    // Parenthesized expressions
    if (parser_check(parser, TOKEN_DELIMITER) && 
        parser->current_token.value && strcmp(parser->current_token.value, "(") == 0) {
        parser_advance(parser);
        
        ASTNode* expr = parse_expression(parser);
        if (!expr) return NULL;
        
        if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after expression")) {
            ast_destroy(expr);
            return NULL;
        }
        
        return expr;
    }
    
    // Struct literals: {field: value, field2: value2}
    if (parser_check(parser, TOKEN_DELIMITER) && 
        parser->current_token.value && strcmp(parser->current_token.value, "{") == 0) {
        parser_advance(parser);
        
        ASTNode* struct_literal = ast_create_node(AST_STRUCT_LITERAL, NULL);
        ast_set_position(struct_literal, parser->current_token.line, parser->current_token.column);
        
        // Parse field initializers
        if (!parser_check(parser, TOKEN_DELIMITER) || 
            strcmp(parser->current_token.value, "}") != 0) {
            
            do {
                // Parse field name
                if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected field name in struct literal");
                    ast_destroy(struct_literal);
                    return NULL;
                }
                
                ASTNode* field_name = ast_create_identifier(parser->current_token.value);
                ast_set_position(field_name, parser->current_token.line, parser->current_token.column);
                parser_advance(parser);
                
                // Expect colon
                if (!parser_check(parser, TOKEN_OPERATOR) || 
                    !parser->current_token.value || strcmp(parser->current_token.value, ":") != 0) {
                    parser_error(parser, "Expected ':' after field name in struct literal");
                    ast_destroy(field_name);
                    ast_destroy(struct_literal);
                    return NULL;
                }
                parser_advance(parser);
                
                // Parse field value
                ASTNode* field_value = parse_expression(parser);
                if (!field_value) {
                    ast_destroy(field_name);
                    ast_destroy(struct_literal);
                    return NULL;
                }
                
                // Create field initializer node
                ASTNode* field_init = ast_create_node(AST_ASSIGNMENT, ":");
                ast_add_child(field_init, field_name);
                ast_add_child(field_init, field_value);
                ast_add_child(struct_literal, field_init);
                
                // Check for comma
                if (parser_check(parser, TOKEN_DELIMITER) && 
                    parser->current_token.value && strcmp(parser->current_token.value, ",") == 0) {
                    parser_advance(parser);
                    continue;
                } else {
                    break;
                }
            } while (true);
        }
        
        // Expect closing brace
        if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '}' after struct literal")) {
            ast_destroy(struct_literal);
            return NULL;
        }
        
        return struct_literal;
    }
    
    parser_error(parser, "Expected expression");
    parser_advance(parser);
    return NULL;
}

// Parse function call
ASTNode* parse_call(Parser* parser, ASTNode* callee) {
    ASTNode* call = ast_create_node(AST_CALL, NULL);
    ast_add_child(call, callee);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '(' for function call")) {
        ast_destroy(call);
        return NULL;
    }
    
    // Parse arguments
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ")") != 0) {
        
        do {
            ASTNode* arg = parse_expression(parser);
            if (!arg) {
                ast_destroy(call);
                return NULL;
            }
            ast_add_child(call, arg);
            
            if (parser_check(parser, TOKEN_DELIMITER) && 
                parser->current_token.value && strcmp(parser->current_token.value, ",") == 0) {
                parser_advance(parser);
                continue;
            } else {
                break;
            }
        } while (true);
    }
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after function arguments")) {
        ast_destroy(call);
        return NULL;
    }
    
    return call;
} 