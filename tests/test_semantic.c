#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/ast/ast.h"
#include "../src/semantic/semantic.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Helper function to run semantic analysis on source code
bool test_semantic_analysis(const char* source, const char* test_name, bool should_pass) {
    printf("\n=== Testing %s ===\n", test_name);
    printf("Source: %s\n", source);
    printf("Expected: %s\n", should_pass ? "PASS" : "FAIL");
    
    Lexer* lexer = lexer_create(source);
    assert(lexer != NULL);
    
    Parser* parser = parser_create(lexer);
    assert(parser != NULL);
    
    ASTNode* ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parse error: %s\n", parser_get_error(parser));
        if (ast) ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    assert(ast != NULL);
    
    SemanticContext* semantic = semantic_create();
    assert(semantic != NULL);
    
    bool result = semantic_analyze(semantic, ast);
    bool has_errors = semantic_has_errors(semantic);
    bool has_warnings = semantic->warning_count > 0;
    
    // For tests expecting failure, we accept either errors or warnings
    bool test_passed = should_pass ? (result && !has_errors) : (!result || has_errors || has_warnings);
    
    if (test_passed) {
        printf("✓ %s test passed!\n", test_name);
    } else {
        printf("✗ %s test failed!\n", test_name);
        printf("  Expected: %s, Got: %s (errors: %d, warnings: %d)\n", 
               should_pass ? "success" : "failure",
               (result && !has_errors) ? "success" : "failure",
               semantic->error_count, semantic->warning_count);
    }
    
    semantic_destroy(semantic);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    return test_passed;
}

// Test valid programs
void test_valid_programs() {
    printf("\n🧪 Testing Valid Programs\n");
    printf("========================\n");
    
    // Simple function
    assert(test_semantic_analysis(
        "fn main() -> i32 { return 42; }",
        "Simple Function", true
    ));
    
    // Function with parameters
    assert(test_semantic_analysis(
        "fn add(i32 a, i32 b) -> i32 { return a + b; }",
        "Function with Parameters", true
    ));
    
    // Variable declaration and usage
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 42; return x; }",
        "Variable Declaration", true
    ));
    
    // Function call
    assert(test_semantic_analysis(
        "fn add(i32 a, i32 b) -> i32 { return a + b; } fn main() -> i32 { return add(2, 3); }",
        "Function Call", true
    ));
    
    // Nested scopes
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 1; { i32 y = 2; x = x + y; } return x; }",
        "Nested Scopes", true
    ));
}

// Test symbol errors
void test_symbol_errors() {
    printf("\n🧪 Testing Symbol Errors\n");
    printf("========================\n");
    
    // Undefined variable
    assert(test_semantic_analysis(
        "fn main() -> i32 { return x; }",
        "Undefined Variable", false
    ));
    
    // Undefined function
    assert(test_semantic_analysis(
        "fn main() -> i32 { return foo(); }",
        "Undefined Function", false
    ));
    
    // Redefined variable in same scope
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 1; i32 x = 2; return x; }",
        "Redefined Variable", false
    ));
    
    // Redefined function
    assert(test_semantic_analysis(
        "fn test() -> i32 { return 1; } fn test() -> i32 { return 2; }",
        "Redefined Function", false
    ));
    
    // Redefined parameter
    assert(test_semantic_analysis(
        "fn test(i32 x, i32 x) -> i32 { return x; }",
        "Redefined Parameter", false
    ));
}

// Test scope rules
void test_scope_rules() {
    printf("\n🧪 Testing Scope Rules\n");
    printf("=====================\n");
    
    // Variable shadowing (should be allowed)
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 1; { i32 x = 2; } return x; }",
        "Variable Shadowing", true
    ));
    
    // Out of scope variable
    assert(test_semantic_analysis(
        "fn main() -> i32 { { i32 x = 1; } return x; }",
        "Out of Scope Variable", false
    ));
    
    // Parameter scope
    assert(test_semantic_analysis(
        "fn test(i32 x) -> i32 { return x; } fn main() -> i32 { return x; }",
        "Parameter Out of Scope", false
    ));
}

// Test function analysis
void test_function_analysis() {
    printf("\n🧪 Testing Function Analysis\n");
    printf("============================\n");
    
    // Missing return (should warn)
    assert(test_semantic_analysis(
        "fn test() -> i32 { i32 x = 42; }",
        "Missing Return", false // This should generate a warning
    ));
    
    // Void function (no return needed)
    assert(test_semantic_analysis(
        "fn test() -> void { i32 x = 42; }",
        "Void Function", true
    ));
    
    // Function with return
    assert(test_semantic_analysis(
        "fn test() -> i32 { return 42; }",
        "Function with Return", true
    ));
}

// Test type checking basics
void test_type_checking() {
    printf("\n🧪 Testing Type Checking\n");
    printf("========================\n");
    
    // Valid variable types
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 42; f32 y = 3.14; bool z = true; return 0; }",
        "Valid Variable Types", true
    ));
    
    // Function call type checking
    assert(test_semantic_analysis(
        "fn add(i32 a, i32 b) -> i32 { return a + b; } fn main() -> i32 { return add(1, 2); }",
        "Function Call Types", true
    ));
}

// Test uninitialized variables
void test_uninitialized_variables() {
    printf("\n🧪 Testing Uninitialized Variables\n");
    printf("==================================\n");
    
    // Uninitialized variable usage (should warn)
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x; return x; }",
        "Uninitialized Variable", false // Should generate warning
    ));
    
    // Initialized variable
    assert(test_semantic_analysis(
        "fn main() -> i32 { i32 x = 42; return x; }",
        "Initialized Variable", true
    ));
    
    // Parameter usage (always initialized)
    assert(test_semantic_analysis(
        "fn test(i32 x) -> i32 { return x; }",
        "Parameter Usage", true
    ));
}

// Main test runner
int main() {
    printf("🚀 Running Echo Semantic Analysis Tests\n");
    printf("=======================================\n");
    
    test_valid_programs();
    test_symbol_errors();
    test_scope_rules();
    test_function_analysis();
    test_type_checking();
    test_uninitialized_variables();
    
    printf("\n🎉 All semantic analysis tests completed!\n");
    printf("Note: Some tests may show warnings - this is expected behavior.\n");
    return 0;
} 