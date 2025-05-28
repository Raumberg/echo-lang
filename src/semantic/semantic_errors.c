#define _GNU_SOURCE
#include "semantic_errors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Error message templates
const char* semantic_error_messages[] = {
    [SEMANTIC_ERROR_NONE] = "No error",
    
    // Type errors
    [SEMANTIC_ERROR_TYPE_MISMATCH] = "Type mismatch",
    [SEMANTIC_ERROR_INCOMPATIBLE_TYPES] = "Incompatible types",
    [SEMANTIC_ERROR_INVALID_OPERATION] = "Invalid operation for type",
    [SEMANTIC_ERROR_INVALID_CAST] = "Invalid type cast",
    
    // Symbol errors
    [SEMANTIC_ERROR_UNDEFINED_SYMBOL] = "Undefined symbol",
    [SEMANTIC_ERROR_REDEFINED_SYMBOL] = "Symbol already defined",
    [SEMANTIC_ERROR_OUT_OF_SCOPE] = "Symbol out of scope",
    
    // Function errors
    [SEMANTIC_ERROR_UNDEFINED_FUNCTION] = "Undefined function",
    [SEMANTIC_ERROR_WRONG_ARGUMENT_COUNT] = "Wrong number of arguments",
    [SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE] = "Wrong argument type",
    [SEMANTIC_ERROR_MISSING_RETURN] = "Missing return statement",
    [SEMANTIC_ERROR_UNREACHABLE_CODE] = "Unreachable code",
    
    // Memory errors
    [SEMANTIC_ERROR_DOUBLE_FREE] = "Double free detected",
    [SEMANTIC_ERROR_MEMORY_LEAK] = "Potential memory leak",
    [SEMANTIC_ERROR_NULL_DEREFERENCE] = "Null pointer dereference",
    [SEMANTIC_ERROR_UNINITIALIZED_VARIABLE] = "Use of uninitialized variable",
    
    // Control flow errors
    [SEMANTIC_ERROR_INVALID_BREAK] = "Break statement outside loop",
    [SEMANTIC_ERROR_INVALID_CONTINUE] = "Continue statement outside loop",
    [SEMANTIC_ERROR_DEAD_CODE] = "Dead code detected"
};

// Create semantic error
SemanticError* semantic_error_create(SemanticErrorType type, SemanticSeverity severity, 
                                   int line, int column, const char* message) {
    SemanticError* error = malloc(sizeof(SemanticError));
    if (!error) return NULL;
    
    error->type = type;
    error->severity = severity;
    error->line = line;
    error->column = column;
    error->message = message ? strdup(message) : NULL;
    error->filename = NULL;
    error->next = NULL;
    
    return error;
}

// Destroy semantic error
void semantic_error_destroy(SemanticError* error) {
    if (!error) return;
    
    free(error->message);
    free(error->filename);
    
    if (error->next) {
        semantic_error_destroy(error->next);
    }
    
    free(error);
}

// Convert error type to string
const char* semantic_error_type_to_string(SemanticErrorType type) {
    if (type >= 0 && type < SEMANTIC_ERROR_COUNT) {
        return semantic_error_messages[type];
    }
    return "Unknown error";
}

// Convert severity to string
const char* semantic_severity_to_string(SemanticSeverity severity) {
    switch (severity) {
        case SEMANTIC_SEVERITY_ERROR: return "ERROR";
        case SEMANTIC_SEVERITY_WARNING: return "WARNING";
        case SEMANTIC_SEVERITY_INFO: return "INFO";
        default: return "UNKNOWN";
    }
} 