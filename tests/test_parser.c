#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/ast/ast.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test helper function
void test_parse_success(const char* source, const char* test_name) {
    printf("\n=== Testing %s ===\n", test_name);
    printf("Source: %s\n", source);
    
    Lexer* lexer = lexer_create(source);
    assert(lexer != NULL);
    
    Parser* parser = parser_create(lexer);
    assert(parser != NULL);
    
    ASTNode* ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("ERROR: %s\n", parser_get_error(parser));
        assert(false);
    }
    
    assert(ast != NULL);
    printf("AST:\n");
    ast_print(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("✓ %s test passed!\n", test_name);
}

// Test simple function
void test_simple_function() {
    const char* source = "fn main() -> i32 { return 42; }";
    test_parse_success(source, "Simple Function");
}

// Test function with parameters
void test_function_with_params() {
    const char* source = "fn add(i32 a, i32 b) -> i32 { return a + b; }";
    test_parse_success(source, "Function with Parameters");
}

// Test variable declaration
void test_variable_declaration() {
    const char* source = "fn main() -> i32 { i32 x = 42; return x; }";
    test_parse_success(source, "Variable Declaration");
}

// Test expressions
void test_expressions() {
    const char* source = "fn main() -> i32 { return 2 + 3 * 4; }";
    test_parse_success(source, "Expressions");
}

// Test alloc and delete
void test_alloc_delete() {
    const char* source = "fn main() -> i32 { i32* ptr = alloc i32(42); delete ptr; return 0; }";
    test_parse_success(source, "Alloc and Delete");
}

// Test function call
void test_function_call() {
    const char* source = "fn main() -> i32 { i32 result = add(2, 3); return result; }";
    test_parse_success(source, "Function Call");
}

// Test with preprocessor
void test_with_preprocessor() {
    const char* source = "#include core::io\nfn main() -> i32 { return 0; }";
    test_parse_success(source, "With Preprocessor");
}

// Test for loop
void test_for_loop() {
    const char* source = "fn main() -> i32 { for (i32 i = 0; i < 10; i++) { return i; } return 0; }";
    test_parse_success(source, "For Loop");
}

// Test error handling
void test_error_handling() {
    printf("\n=== Testing Error Handling ===\n");
    
    const char* bad_source = "fn main( { return 42; }"; // Missing closing paren
    printf("Source: %s\n", bad_source);
    
    Lexer* lexer = lexer_create(bad_source);
    Parser* parser = parser_create(lexer);
    
    ASTNode* ast = parser_parse(parser);
    
    // Check if there were any errors during parsing (even if recovered)
    if (parser_get_error(parser) != NULL) {
        printf("Expected error caught: %s\n", parser_get_error(parser));
        printf("✓ Error handling test passed!\n");
    } else {
        printf("ERROR: Expected parse error but no errors detected\n");
        assert(false);
    }
    
    if (ast) ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

// Main test runner
int main() {
    printf("🚀 Running Echo Parser Tests\n");
    printf("=============================\n");
    
    test_simple_function();
    test_function_with_params();
    test_variable_declaration();
    test_expressions();
    test_alloc_delete();
    test_function_call();
    test_with_preprocessor();
    test_for_loop();
    test_error_handling();
    
    printf("\n🎉 All parser tests passed!\n");
    return 0;
} 