#include "../src/lexer.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test helper function
void test_token(Token token, TokenType expected_type, const char* expected_value) {
    assert(token.type == expected_type);
    if (expected_value) {
        assert(token.value != NULL);
        assert(strcmp(token.value, expected_value) == 0);
    }
    printf("✓ Token: %s (type: %d)\n", token.value ? token.value : "NULL", token.type);
}

// Test basic tokens
void test_basic_tokens() {
    printf("\n=== Testing Basic Tokens ===\n");
    
    const char* source = "fn main() -> i32 { return 42; }";
    Lexer* lexer = lexer_create(source);
    
    Token token;
    
    // fn
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_KEYWORD, "fn");
    token_destroy(&token);
    
    // main
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_IDENTIFIER, "main");
    token_destroy(&token);
    
    // (
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_DELIMITER, "(");
    token_destroy(&token);
    
    // )
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_DELIMITER, ")");
    token_destroy(&token);
    
    // ->
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_OPERATOR, "->");
    token_destroy(&token);
    
    // i32
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_KEYWORD, "i32");
    token_destroy(&token);
    
    // {
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_DELIMITER, "{");
    token_destroy(&token);
    
    // return
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_KEYWORD, "return");
    token_destroy(&token);
    
    // 42
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_INTEGER, "42");
    token_destroy(&token);
    
    // ;
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_DELIMITER, ";");
    token_destroy(&token);
    
    // }
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_DELIMITER, "}");
    token_destroy(&token);
    
    // EOF
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_EOF, NULL);
    token_destroy(&token);
    
    lexer_destroy(lexer);
    printf("✓ Basic tokens test passed!\n");
}

// Test numbers
void test_numbers() {
    printf("\n=== Testing Numbers ===\n");
    
    const char* source = "42 3.14 0xFF 0b1010 0o755 1.23e-4 3.14f";
    Lexer* lexer = lexer_create(source);
    
    Token token;
    
    // 42
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_INTEGER, "42");
    token_destroy(&token);
    
    // 3.14
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_FLOAT, "3.14");
    token_destroy(&token);
    
    // 0xFF
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_INTEGER, "0xFF");
    token_destroy(&token);
    
    // 0b1010
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_INTEGER, "0b1010");
    token_destroy(&token);
    
    // 0o755
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_INTEGER, "0o755");
    token_destroy(&token);
    
    // 1.23e-4
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_FLOAT, "1.23e-4");
    token_destroy(&token);
    
    // 3.14f
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_FLOAT, "3.14f");
    token_destroy(&token);
    
    lexer_destroy(lexer);
    printf("✓ Numbers test passed!\n");
}

// Test strings
void test_strings() {
    printf("\n=== Testing Strings ===\n");
    
    const char* source = "\"Hello, World!\" \"Line\\nBreak\" 'c' '\\n'";
    Lexer* lexer = lexer_create(source);
    
    Token token;
    
    // "Hello, World!"
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_STRING, "Hello, World!");
    token_destroy(&token);
    
    // "Line\nBreak"
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_STRING, "Line\nBreak");
    token_destroy(&token);
    
    // 'c'
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_CHAR, "c");
    token_destroy(&token);
    
    // '\n'
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_CHAR, "\\n");
    token_destroy(&token);
    
    lexer_destroy(lexer);
    printf("✓ Strings test passed!\n");
}

// Test operators
void test_operators() {
    printf("\n=== Testing Operators ===\n");
    
    const char* source = "+ - * / % == != <= >= && || ++ -- += -> ::";
    Lexer* lexer = lexer_create(source);
    
    const char* expected_ops[] = {
        "+", "-", "*", "/", "%", "==", "!=", "<=", ">=", 
        "&&", "||", "++", "--", "+=", "->", "::"
    };
    
    for (int i = 0; i < 16; i++) {
        Token token = lexer_next_token(lexer);
        test_token(token, TOKEN_OPERATOR, expected_ops[i]);
        token_destroy(&token);
    }
    
    lexer_destroy(lexer);
    printf("✓ Operators test passed!\n");
}

// Test preprocessor
void test_preprocessor() {
    printf("\n=== Testing Preprocessor ===\n");
    
    const char* source = "#include core::io\n#if __dbg__\n#define MAX(a,b) ((a)>(b)?(a):(b))";
    Lexer* lexer = lexer_create(source);
    
    Token token;
    
    // #include core::io
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_PREPROCESSOR, "#include core::io");
    token_destroy(&token);
    
    // #if __dbg__
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_PREPROCESSOR, "#if __dbg__");
    token_destroy(&token);
    
    // #define MAX(a,b) ((a)>(b)?(a):(b))
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_PREPROCESSOR, "#define MAX(a,b) ((a)>(b)?(a):(b))");
    token_destroy(&token);
    
    lexer_destroy(lexer);
    printf("✓ Preprocessor test passed!\n");
}

// Test comments
void test_comments() {
    printf("\n=== Testing Comments ===\n");
    
    const char* source = "fn main() {\n    // This is a comment\n    i32 x = 42; /* block comment */ return x;\n}";
    Lexer* lexer = lexer_create(source);
    
    Token token;
    
    // fn
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_KEYWORD, "fn");
    token_destroy(&token);
    
    // main
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_IDENTIFIER, "main");
    token_destroy(&token);
    
    // Skip to i32 (comments should be ignored)
    token = lexer_next_token(lexer); // (
    token_destroy(&token);
    token = lexer_next_token(lexer); // )
    token_destroy(&token);
    token = lexer_next_token(lexer); // {
    token_destroy(&token);
    
    // i32
    token = lexer_next_token(lexer);
    test_token(token, TOKEN_KEYWORD, "i32");
    token_destroy(&token);
    
    lexer_destroy(lexer);
    printf("✓ Comments test passed!\n");
}

// Test complete Echo program
void test_echo_program() {
    printf("\n=== Testing Complete Echo Program ===\n");
    
    const char* source = 
        "#include core::io\n"
        "\n"
        "fn main() -> i32 {\n"
        "    string message = \"Hello, Echo!\"\n"
        "    i32* ptr = alloc i32(42)\n"
        "    delete ptr\n"
        "    return 0\n"
        "}";
    
    Lexer* lexer = lexer_create(source);
    Token token;
    int token_count = 0;
    
    do {
        token = lexer_next_token(lexer);
        if (token.type != TOKEN_EOF) {
            printf("Token %d: %s (type: %d, line: %d, col: %d)\n", 
                   ++token_count, token.value ? token.value : "NULL", 
                   token.type, token.line, token.column);
        }
        token_destroy(&token);
    } while (token.type != TOKEN_EOF);
    
    lexer_destroy(lexer);
    printf("✓ Complete program test passed! (%d tokens)\n", token_count);
}

// Main test runner
int main() {
    printf("🚀 Running Echo Lexer Tests\n");
    printf("============================\n");
    
    test_basic_tokens();
    test_numbers();
    test_strings();
    test_operators();
    test_preprocessor();
    test_comments();
    test_echo_program();
    
    printf("\n🎉 All tests passed!\n");
    return 0;
} 