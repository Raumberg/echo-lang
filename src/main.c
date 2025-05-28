#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "semantic/semantic.h"
#include "codegen/codegen.h"

// Read file contents
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(length + 1);
    if (!content) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    fread(content, 1, length, file);
    content[length] = '\0';
    
    fclose(file);
    return content;
}

// Generate output filename
char* generate_output_filename(const char* input_filename) {
    if (!input_filename) return NULL;
    
    // Find the last dot to replace extension
    const char* dot = strrchr(input_filename, '.');
    const char* slash = strrchr(input_filename, '/');
    
    // If no dot found or dot is before last slash, append .c
    if (!dot || (slash && dot < slash)) {
        size_t len = strlen(input_filename);
        char* output = malloc(len + 3); // +2 for ".c" +1 for null
        if (!output) return NULL;
        strcpy(output, input_filename);
        strcat(output, ".c");
        return output;
    }
    
    // Replace extension with .c
    size_t base_len = dot - input_filename;
    char* output = malloc(base_len + 3); // +2 for ".c" +1 for null
    if (!output) return NULL;
    
    strncpy(output, input_filename, base_len);
    output[base_len] = '\0';
    strcat(output, ".c");
    
    return output;
}

int main(int argc, char* argv[]) {
    printf("Echo Language Compiler v1.0\n");
    printf("===========================\n\n");
    
    if (argc != 2) {
        printf("Usage: %s <echo_file>\n", argv[0]);
        printf("Example: %s examples/hello.ec\n", argv[0]);
        return 1;
    }
    
    char* source = read_file(argv[1]);
    if (!source) {
        return 1;
    }
    
    printf("Compiling file: %s\n", argv[1]);
    printf("Source code:\n");
    printf("------------\n");
    printf("%s\n", source);
    printf("------------\n\n");
    
    // Create lexer
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    
    // Create parser
    Parser* parser = parser_create(lexer);
    if (!parser) {
        printf("Error: Failed to create parser\n");
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("Parsing...\n");
    printf("----------\n");
    
    // Parse the program
    ASTNode* ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parse failed with errors:\n");
        printf("%s\n", parser_get_error(parser));
        
        if (ast) {
            ast_destroy(ast);
        }
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    if (!ast) {
        printf("Error: Failed to parse program\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("Parse successful! AST:\n");
    printf("----------------------\n");
    ast_print(ast, 0);
    printf("\n");
    
    // Semantic analysis
    printf("Semantic Analysis...\n");
    printf("-------------------\n");
    
    SemanticContext* semantic = semantic_create();
    if (!semantic) {
        printf("Error: Failed to create semantic analyzer\n");
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Set filename for error reporting
    semantic->current_filename = strdup(argv[1]);
    
    bool semantic_ok = semantic_analyze(semantic, ast);
    
    if (!semantic_ok || semantic_has_errors(semantic)) {
        printf("\n❌ Compilation failed due to semantic errors\n");
        semantic_destroy(semantic);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Code generation
    printf("\nCode Generation...\n");
    printf("-----------------\n");
    
    // Generate output filename
    char* output_filename = generate_output_filename(argv[1]);
    if (!output_filename) {
        printf("Error: Failed to generate output filename\n");
        semantic_destroy(semantic);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("Output file: %s\n", output_filename);
    
    // Open output file
    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        printf("Error: Cannot create output file '%s'\n", output_filename);
        free(output_filename);
        semantic_destroy(semantic);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Create code generator
    CodeGenerator* codegen = codegen_create(output_file, semantic->symbol_table);
    if (!codegen) {
        printf("Error: Failed to create code generator\n");
        fclose(output_file);
        free(output_filename);
        semantic_destroy(semantic);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Generate C code
    CodegenResult result = codegen_generate(codegen, ast);
    
    if (result != CODEGEN_SUCCESS) {
        printf("❌ Code generation failed: %s\n", codegen_result_to_string(result));
        codegen_destroy(codegen);
        fclose(output_file);
        free(output_filename);
        semantic_destroy(semantic);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Close output file
    fclose(output_file);
    
    printf("\n🎉 Compilation completed successfully!\n");
    printf("Generated C code: %s\n", output_filename);
    printf("Next step: Compile with gcc\n");
    printf("  gcc -o program %s\n", output_filename);
    
    // Cleanup
    codegen_destroy(codegen);
    free(output_filename);
    semantic_destroy(semantic);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(source);
    
    return 0;
} 