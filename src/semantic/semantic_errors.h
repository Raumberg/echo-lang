#ifndef SEMANTIC_ERRORS_H
#define SEMANTIC_ERRORS_H

// Types of semantic errors
typedef enum {
    SEMANTIC_ERROR_NONE = 0,
    
    // Type errors
    SEMANTIC_ERROR_TYPE_MISMATCH,
    SEMANTIC_ERROR_INCOMPATIBLE_TYPES,
    SEMANTIC_ERROR_INVALID_OPERATION,
    SEMANTIC_ERROR_INVALID_CAST,
    SEMANTIC_ERROR_UNDEFINED_FIELD,       // Field doesn't exist in struct
    SEMANTIC_ERROR_UNDEFINED_TYPE,        // Type doesn't exist
    SEMANTIC_ERROR_INVALID_AUTO_USAGE,    // auto used in invalid context (e.g., struct fields)
    
    // Symbol errors
    SEMANTIC_ERROR_UNDEFINED_SYMBOL,
    SEMANTIC_ERROR_REDEFINED_SYMBOL,
    SEMANTIC_ERROR_OUT_OF_SCOPE,
    
    // Function errors
    SEMANTIC_ERROR_UNDEFINED_FUNCTION,
    SEMANTIC_ERROR_WRONG_ARGUMENT_COUNT,
    SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE,
    SEMANTIC_ERROR_MISSING_RETURN,
    SEMANTIC_ERROR_UNREACHABLE_CODE,
    
    // Memory errors
    SEMANTIC_ERROR_DOUBLE_FREE,
    SEMANTIC_ERROR_MEMORY_LEAK,
    SEMANTIC_ERROR_NULL_DEREFERENCE,
    SEMANTIC_ERROR_UNINITIALIZED_VARIABLE,
    
    // Control flow errors
    SEMANTIC_ERROR_INVALID_BREAK,
    SEMANTIC_ERROR_INVALID_CONTINUE,
    SEMANTIC_ERROR_DEAD_CODE,
    
    SEMANTIC_ERROR_COUNT
} SemanticErrorType;

// Error severity levels
typedef enum {
    SEMANTIC_SEVERITY_ERROR,
    SEMANTIC_SEVERITY_WARNING,
    SEMANTIC_SEVERITY_INFO
} SemanticSeverity;

// Semantic error structure
typedef struct SemanticError {
    SemanticErrorType type;
    SemanticSeverity severity;
    char* message;
    int line;
    int column;
    char* filename;
    struct SemanticError* next;
} SemanticError;

// Error message templates
extern const char* semantic_error_messages[];

// Function declarations
SemanticError* semantic_error_create(SemanticErrorType type, SemanticSeverity severity, 
                                   int line, int column, const char* message);
void semantic_error_destroy(SemanticError* error);
const char* semantic_error_type_to_string(SemanticErrorType type);
const char* semantic_severity_to_string(SemanticSeverity severity);

#endif // SEMANTIC_ERRORS_H 