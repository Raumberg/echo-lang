#define _GNU_SOURCE
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Parse program (top level)
ASTNode* parse_program(Parser* parser) {
    ASTNode* program = ast_create_node(AST_PROGRAM, NULL);
    if (!program) return NULL;
    
    // Skip preprocessor directives at the beginning
    while (parser_check(parser, TOKEN_PREPROCESSOR)) {
        ASTNode* preprocessor = ast_create_node(AST_PREPROCESSOR, parser->current_token.value);
        ast_set_position(preprocessor, parser->current_token.line, parser->current_token.column);
        ast_add_child(program, preprocessor);
        parser_advance(parser);
    }
    
    // Parse top-level declarations
    while (!parser_check(parser, TOKEN_EOF)) {
        if (parser->error_count > 10) {
            printf("Too many errors, stopping parse\n");
            break;
        }
        
        ASTNode* decl = NULL;
        
        if (parser_check(parser, TOKEN_KEYWORD)) {
            const char* kw = parser->current_token.value;
            if (kw && strcmp(kw, "fn") == 0) {
                decl = parse_function(parser);
            } else if (kw && strcmp(kw, "struct") == 0) {
                // TODO: implement struct parsing
                parser_error(parser, "Struct parsing not implemented yet");
                parser_synchronize(parser);
                continue;
            } else if (kw && strcmp(kw, "enum") == 0) {
                // TODO: implement enum parsing
                parser_error(parser, "Enum parsing not implemented yet");
                parser_synchronize(parser);
                continue;
            } else {
                parser_error(parser, "Expected function, struct, or enum declaration");
                parser_synchronize(parser);
                continue;
            }
        } else if (parser_check(parser, TOKEN_PREPROCESSOR)) {
            decl = ast_create_node(AST_PREPROCESSOR, parser->current_token.value);
            ast_set_position(decl, parser->current_token.line, parser->current_token.column);
            parser_advance(parser);
        } else {
            parser_error(parser, "Expected declaration");
            parser_synchronize(parser);
            continue;
        }
        
        if (decl) {
            ast_add_child(program, decl);
        }
        
        if (parser_has_error(parser)) {
            parser_synchronize(parser);
        }
    }
    
    return program;
}

// Parse function declaration
ASTNode* parse_function(Parser* parser) {
    if (!parser_expect_keyword(parser, "fn")) {
        return NULL;
    }
    
    // Function name
    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected function name");
        return NULL;
    }
    
    ASTNode* function = ast_create_node(AST_FUNCTION, parser->current_token.value);
    ast_set_position(function, parser->current_token.line, parser->current_token.column);
    parser_advance(parser);
    
    // Parameters
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '(' after function name")) {
        ast_destroy(function);
        return NULL;
    }
    
    ASTNode* params = ast_create_node(AST_PARAMETER, NULL);
    
    // Parse parameter list
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ")") != 0) {
        
        do {
            // Parse type
            ASTNode* param_type = parse_type(parser);
            if (!param_type) {
                ast_destroy(params);
                ast_destroy(function);
                return NULL;
            }
            
            // Parse parameter name
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected parameter name");
                ast_destroy(param_type);
                ast_destroy(params);
                ast_destroy(function);
                return NULL;
            }
            
            ASTNode* param = ast_create_node(AST_PARAMETER, parser->current_token.value);
            ast_set_position(param, parser->current_token.line, parser->current_token.column);
            ast_add_child(param, param_type);
            ast_add_child(params, param);
            
            parser_advance(parser);
            
            // Check for comma before consuming it
            if (parser_check(parser, TOKEN_DELIMITER) && 
                parser->current_token.value && strcmp(parser->current_token.value, ",") == 0) {
                parser_advance(parser); // consume comma
                continue;
            } else {
                break;
            }
        } while (true);
    }
    
    ast_add_child(function, params);
    
    // Closing parenthesis
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after parameters")) {
        ast_destroy(function);
        return NULL;
    }
    
    // Return type
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && strcmp(parser->current_token.value, "->") == 0) {
        parser_advance(parser);
        
        ASTNode* return_type = parse_type(parser);
        if (!return_type) {
            ast_destroy(function);
            return NULL;
        }
        ast_add_child(function, return_type);
    }
    
    // Function body
    ASTNode* body = parse_block(parser);
    if (!body) {
        ast_destroy(function);
        return NULL;
    }
    ast_add_child(function, body);
    
    return function;
}

// Parse type
ASTNode* parse_type(Parser* parser) {
    if (!parser_check(parser, TOKEN_KEYWORD)) {
        parser_error(parser, "Expected type");
        return NULL;
    }
    
    const char* type_name = parser->current_token.value;
    if (!is_type_keyword(type_name)) {
        parser_error(parser, "Expected valid type");
        return NULL;
    }
    
    ASTNode* type_node = ast_create_node(AST_TYPE, type_name);
    ast_set_position(type_node, parser->current_token.line, parser->current_token.column);
    parser_advance(parser);
    
    // Check for pointer
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && strcmp(parser->current_token.value, "*") == 0) {
        type_node->is_pointer = true;
        parser_advance(parser);
    }
    
    // Check for optional
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && strcmp(parser->current_token.value, "?") == 0) {
        type_node->is_optional = true;
        parser_advance(parser);
    }
    
    return type_node;
}

// Parse block statement
ASTNode* parse_block(Parser* parser) {
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '{'")) {
        return NULL;
    }
    
    ASTNode* block = ast_create_node(AST_BLOCK, NULL);
    ast_set_position(block, parser->current_token.line, parser->current_token.column);
    
    while (!parser_check(parser, TOKEN_DELIMITER) || 
           strcmp(parser->current_token.value, "}") != 0) {
        
        if (parser_check(parser, TOKEN_EOF)) {
            parser_error(parser, "Unexpected end of file in block");
            ast_destroy(block);
            return NULL;
        }
        
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(block, stmt);
        }
        
        if (parser_has_error(parser)) {
            parser_synchronize(parser);
        }
    }
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '}'")) {
        ast_destroy(block);
        return NULL;
    }
    
    return block;
}

// Parse statement
ASTNode* parse_statement(Parser* parser) {
    if (parser_check(parser, TOKEN_KEYWORD)) {
        const char* kw = parser->current_token.value;
        
        if (kw && strcmp(kw, "return") == 0) {
            return parse_return_statement(parser);
        } else if (kw && strcmp(kw, "if") == 0) {
            return parse_if_statement(parser);
        } else if (kw && strcmp(kw, "for") == 0) {
            return parse_for_statement(parser);
        } else if (kw && strcmp(kw, "while") == 0) {
            return parse_while_statement(parser);
        } else if (is_type_keyword(kw)) {
            return parse_variable_declaration(parser);
        }
    }
    
    if (parser_check(parser, TOKEN_DELIMITER) && 
        parser->current_token.value && strcmp(parser->current_token.value, "{") == 0) {
        return parse_block(parser);
    }
    
    // Expression statement
    ASTNode* expr = parse_expression(parser);
    if (!expr) return NULL;
    
    ASTNode* expr_stmt = ast_create_node(AST_EXPRESSION_STMT, NULL);
    ast_add_child(expr_stmt, expr);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ';' after expression")) {
        ast_destroy(expr_stmt);
        return NULL;
    }
    
    return expr_stmt;
}

// Parse return statement
ASTNode* parse_return_statement(Parser* parser) {
    if (!parser_expect_keyword(parser, "return")) {
        return NULL;
    }
    
    ASTNode* return_stmt = ast_create_node(AST_RETURN, NULL);
    ast_set_position(return_stmt, parser->current_token.line, parser->current_token.column);
    
    // Optional return value
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ";") != 0) {
        ASTNode* expr = parse_expression(parser);
        if (expr) {
            ast_add_child(return_stmt, expr);
        }
    }
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ';' after return statement")) {
        ast_destroy(return_stmt);
        return NULL;
    }
    
    return return_stmt;
}

// Parse variable declaration
ASTNode* parse_variable_declaration(Parser* parser) {
    ASTNode* type_node = parse_type(parser);
    if (!type_node) return NULL;
    
    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected variable name");
        ast_destroy(type_node);
        return NULL;
    }
    
    ASTNode* var_decl = ast_create_node(AST_VARIABLE_DECL, parser->current_token.value);
    ast_set_position(var_decl, parser->current_token.line, parser->current_token.column);
    ast_add_child(var_decl, type_node);
    
    parser_advance(parser);
    
    // Optional initialization
    if (parser_check(parser, TOKEN_OPERATOR) && 
        parser->current_token.value && strcmp(parser->current_token.value, "=") == 0) {
        parser_advance(parser);
        
        ASTNode* init_expr = parse_expression(parser);
        if (init_expr) {
            ast_add_child(var_decl, init_expr);
        }
    }
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ';' after variable declaration")) {
        ast_destroy(var_decl);
        return NULL;
    }
    
    return var_decl;
}

// Parse if statement
ASTNode* parse_if_statement(Parser* parser) {
    if (!parser_expect_keyword(parser, "if")) {
        return NULL;
    }
    
    ASTNode* if_stmt = ast_create_node(AST_IF, NULL);
    ast_set_position(if_stmt, parser->current_token.line, parser->current_token.column);
    
    // Parse condition in parentheses
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '(' after 'if'")) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        ast_destroy(if_stmt);
        return NULL;
    }
    ast_add_child(if_stmt, condition);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after if condition")) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    // Parse then block
    ASTNode* then_block = parse_statement(parser);
    if (!then_block) {
        ast_destroy(if_stmt);
        return NULL;
    }
    ast_add_child(if_stmt, then_block);
    
    // Optional else
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && strcmp(parser->current_token.value, "else") == 0) {
        parser_advance(parser);
        
        ASTNode* else_block = parse_statement(parser);
        if (else_block) {
            ast_add_child(if_stmt, else_block);
        }
    }
    
    return if_stmt;
}

ASTNode* parse_for_statement(Parser* parser) {
    if (!parser_expect_keyword(parser, "for")) {
        return NULL;
    }
    
    ASTNode* for_stmt = ast_create_node(AST_FOR, NULL);
    ast_set_position(for_stmt, parser->current_token.line, parser->current_token.column);
    
    // Parse (init; condition; increment)
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '(' after 'for'")) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    // Init statement (can be variable declaration or expression)
    ASTNode* init = NULL;
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ";") != 0) {
        
        if (parser_check(parser, TOKEN_KEYWORD) && is_type_keyword(parser->current_token.value)) {
            init = parse_variable_declaration(parser);
        } else {
            init = parse_expression(parser);
            if (init && !parser_expect(parser, TOKEN_DELIMITER, "Expected ';' after for init")) {
                ast_destroy(init);
                ast_destroy(for_stmt);
                return NULL;
            }
        }
    } else {
        parser_advance(parser); // consume ';'
    }
    
    if (init) ast_add_child(for_stmt, init);
    
    // Condition
    ASTNode* condition = NULL;
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ";") != 0) {
        condition = parse_expression(parser);
    }
    if (condition) ast_add_child(for_stmt, condition);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ';' after for condition")) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    // Increment
    ASTNode* increment = NULL;
    if (!parser_check(parser, TOKEN_DELIMITER) || 
        strcmp(parser->current_token.value, ")") != 0) {
        increment = parse_expression(parser);
    }
    if (increment) ast_add_child(for_stmt, increment);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after for increment")) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    // Parse body
    ASTNode* body = parse_statement(parser);
    if (!body) {
        ast_destroy(for_stmt);
        return NULL;
    }
    ast_add_child(for_stmt, body);
    
    return for_stmt;
}

ASTNode* parse_while_statement(Parser* parser) {
    if (!parser_expect_keyword(parser, "while")) {
        return NULL;
    }
    
    ASTNode* while_stmt = ast_create_node(AST_WHILE, NULL);
    ast_set_position(while_stmt, parser->current_token.line, parser->current_token.column);
    
    // Parse condition in parentheses
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected '(' after 'while'")) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        ast_destroy(while_stmt);
        return NULL;
    }
    ast_add_child(while_stmt, condition);
    
    if (!parser_expect(parser, TOKEN_DELIMITER, "Expected ')' after while condition")) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    // Parse body
    ASTNode* body = parse_statement(parser);
    if (!body) {
        ast_destroy(while_stmt);
        return NULL;
    }
    ast_add_child(while_stmt, body);
    
    return while_stmt;
} 