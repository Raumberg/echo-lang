# Step 4: Code Generation (C Transpilation)

## Overview
Implement code generation by transpiling Echo language to C code. This approach allows us to leverage existing C compilers and provides a quick path to a working compiler.

## Goals
- ✅ Generate valid C code from Echo AST
- ✅ Handle all Echo language constructs
- ✅ Maintain type safety and memory management
- ✅ Support Echo's unique features (optional types, smart pointers)
- ✅ Generate readable and debuggable C code

## Architecture

### Code Generator Structure
```c
// Main code generator
typedef struct CodeGenerator {
    FILE* output;                    // Output C file
    SymbolTable* symbol_table;       // Symbol information
    int indent_level;                // Current indentation
    bool in_function;                // Are we inside a function?
    char* current_function_name;     // Current function name
    int temp_var_counter;            // For generating temp variables
    int label_counter;               // For generating labels
} CodeGenerator;
```

### Translation Strategy

#### 1. Types Mapping
```
Echo Type    -> C Type
i32          -> int32_t
i64          -> int64_t
f32          -> float
f64          -> double
bool         -> bool
string       -> char*
T*           -> T*
T?           -> struct { bool has_value; T value; }
unique<T>    -> T* (with ownership tracking)
shared<T>    -> struct { T* ptr; int* ref_count; }
```

#### 2. Memory Management
```c
// Smart pointer implementations
typedef struct {
    void* ptr;
    void (*destructor)(void*);
} unique_ptr;

typedef struct {
    void* ptr;
    int* ref_count;
    void (*destructor)(void*);
} shared_ptr;

// Generated helper functions
unique_ptr* unique_create(size_t size, void (*destructor)(void*));
void unique_destroy(unique_ptr* ptr);
shared_ptr* shared_create(size_t size, void (*destructor)(void*));
shared_ptr* shared_copy(shared_ptr* ptr);
void shared_destroy(shared_ptr* ptr);
```

#### 3. Optional Types
```c
// Optional type implementation
#define OPTIONAL(T) struct { bool has_value; T value; }
#define SOME(val) { .has_value = true, .value = (val) }
#define NONE { .has_value = false }
#define IS_SOME(opt) ((opt).has_value)
#define UNWRAP(opt) ((opt).value)
```

## Implementation Plan

### Phase 1: Basic Code Generation (Days 1-2)
- [x] Create code generator structure
- [x] Implement basic AST traversal
- [x] Generate function declarations and definitions
- [x] Handle basic types and variables
- [x] Generate expressions and statements

### Phase 2: Advanced Features (Days 3-4)
- [x] Implement optional types
- [x] Add smart pointer support
- [x] Handle memory allocation/deallocation
- [x] Generate control flow structures

### Phase 3: Runtime Support (Day 5)
- [x] Create runtime library headers
- [x] Implement helper functions
- [x] Add error handling
- [x] Generate main function wrapper

### Phase 4: Testing & Integration (Days 6-7)
- [x] Unit tests for code generator
- [x] End-to-end compilation tests
- [x] Performance optimization
- [x] Documentation and examples

## File Structure
```
src/codegen/
├── codegen.h           # Main code generator interface
├── codegen.c           # Core code generation logic
├── c_types.h           # C type mapping utilities
├── c_types.c           # Type conversion functions
├── runtime.h           # Runtime library interface
├── runtime.c           # Runtime helper functions
└── templates/          # C code templates
    ├── runtime.c.template
    ├── optional.h.template
    └── smart_ptr.h.template
```

## Generated C Code Structure
```c
// Generated file: output.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "echo_runtime.h"

// Type definitions
OPTIONAL(int32_t) optional_i32;

// Function declarations
int32_t add(int32_t a, int32_t b);
int32_t main(void);

// Function implementations
int32_t add(int32_t a, int32_t b) {
    return a + b;
}

int32_t main(void) {
    int32_t result = add(2, 3);
    return result;
}
```

## Key Features to Implement

### 1. Function Generation
- Function signatures with proper C types
- Parameter handling
- Return value management
- Local variable declarations

### 2. Expression Generation
- Arithmetic and logical operations
- Function calls
- Variable access
- Type casting

### 3. Statement Generation
- Variable declarations
- Assignments
- Control flow (if, for, while)
- Return statements

### 4. Memory Management
- Automatic cleanup for smart pointers
- Reference counting for shared_ptr
- Scope-based destruction

### 5. Error Handling
- Null pointer checks
- Optional type validation
- Runtime error messages

## Testing Strategy

### Unit Tests
- Test each AST node type generation
- Verify C code syntax correctness
- Check type mapping accuracy
- Validate memory management

### Integration Tests
- Compile generated C code with gcc
- Run generated executables
- Compare output with expected results
- Performance benchmarks

### Example Test Cases
```echo
// Test 1: Basic function
fn add(i32 a, i32 b) -> i32 {
    return a + b;
}

// Test 2: Optional types
fn safe_divide(i32 a, i32 b) -> i32? {
    if (b == 0) return null;
    return a / b;
}

// Test 3: Memory management
fn create_array() -> unique<i32> {
    return alloc i32(42);
}
```

## Success Criteria
- ✅ Generate syntactically correct C code
- ✅ All Echo language features supported
- ✅ Generated code compiles with gcc
- ✅ Runtime tests pass
- ✅ Memory management works correctly
- ✅ Performance is acceptable

## Estimated Timeline: 7 days
- Day 1-2: Basic code generation
- Day 3-4: Advanced features
- Day 5: Runtime support
- Day 6-7: Testing and polish

## Next Steps After Completion
- Step 5: Optimization and LLVM backend
- Step 6: Standard library
- Step 7: Package management
- Step 8: IDE integration 