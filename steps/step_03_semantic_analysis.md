# Step 3: Semantic Analysis

## Цель
Реализовать семантический анализатор для проверки корректности программы на Echo после парсинга.

## Задачи

### 3.1 Symbol Table (Таблица символов)
- **Scope management** - управление областями видимости
- **Variable declarations** - отслеживание объявлений переменных
- **Function signatures** - сигнатуры функций
- **Type definitions** - определения типов (struct, enum)
- **Nested scopes** - вложенные области видимости

### 3.2 Type Checking (Проверка типов)
- **Type compatibility** - совместимость типов в выражениях
- **Function call validation** - проверка вызовов функций
- **Assignment validation** - проверка присваиваний
- **Pointer arithmetic** - арифметика указателей
- **Array bounds** - проверка границ массивов (базовая)

### 3.3 Control Flow Analysis
- **Return path validation** - все пути возвращают значение
- **Unreachable code detection** - обнаружение недостижимого кода
- **Variable initialization** - проверка инициализации переменных
- **Break/continue validation** - корректность break/continue

### 3.4 Memory Management Validation
- **Alloc/delete pairing** - соответствие alloc и delete
- **Double free detection** - обнаружение двойного освобождения
- **Memory leak warnings** - предупреждения о утечках памяти
- **Null pointer dereference** - разыменование null указателей

## Структура файлов

```
src/semantic/
├── semantic.h              # Основной интерфейс
├── semantic.c              # Главный анализатор
├── symbol_table.h          # Таблица символов
├── symbol_table.c          
├── type_checker.h          # Проверка типов
├── type_checker.c          
├── scope_manager.h         # Управление областями видимости
├── scope_manager.c         
└── semantic_errors.h       # Типы семантических ошибок
```

## Основные структуры данных

### Symbol Table Entry
```c
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_TYPE
} SymbolType;

typedef struct Symbol {
    char* name;
    SymbolType type;
    ASTNode* declaration;
    ASTNode* type_node;
    int scope_level;
    bool is_initialized;
    bool is_used;
    struct Symbol* next;
} Symbol;
```

### Scope
```c
typedef struct Scope {
    Symbol* symbols;
    struct Scope* parent;
    int level;
    bool is_function_scope;
} Scope;
```

### Semantic Context
```c
typedef struct SemanticContext {
    Scope* current_scope;
    ASTNode* current_function;
    bool has_error;
    int error_count;
    char* error_message;
} SemanticContext;
```

## Этапы реализации

### Этап 1: Symbol Table (2-3 дня)
1. Реализовать базовую структуру Symbol и Scope
2. Функции создания/уничтожения областей видимости
3. Добавление/поиск символов
4. Обход AST для сбора символов

### Этап 2: Type System (2-3 дня)
1. Определение базовых типов Echo
2. Проверка совместимости типов
3. Вывод типов для выражений
4. Проверка типов в присваиваниях

### Этап 3: Function Analysis (1-2 дня)
1. Проверка сигнатур функций
2. Валидация вызовов функций
3. Проверка возвращаемых значений
4. Анализ параметров

### Этап 4: Control Flow (1-2 дня)
1. Анализ путей выполнения
2. Проверка return statements
3. Валидация break/continue
4. Обнаружение недостижимого кода

### Этап 5: Memory Management (1-2 дня)
1. Отслеживание alloc/delete
2. Анализ жизненного цикла указателей
3. Предупреждения о потенциальных утечках
4. Проверка разыменования указателей

## Тестирование

### Unit Tests
```c
// tests/semantic/
├── test_symbol_table.c
├── test_type_checker.c
├── test_scope_manager.c
└── test_semantic_analysis.c
```

### Test Cases
1. **Valid programs** - корректные программы должны проходить анализ
2. **Type errors** - ошибки типов должны обнаруживаться
3. **Scope errors** - ошибки областей видимости
4. **Function errors** - ошибки в функциях
5. **Memory errors** - ошибки управления памятью

## Примеры ошибок для обнаружения

### Type Errors
```echo
fn main() -> i32 {
    i32 x = "string";  // Error: incompatible types
    return x + true;   // Error: invalid operation
}
```

### Scope Errors
```echo
fn main() -> i32 {
    if (true) {
        i32 x = 42;
    }
    return x;  // Error: x not in scope
}
```

### Function Errors
```echo
fn add(i32 a, i32 b) -> i32 {
    // Error: missing return statement
}

fn main() -> i32 {
    return add(1, 2, 3);  // Error: too many arguments
}
```

### Memory Errors
```echo
fn main() -> i32 {
    i32* ptr = alloc i32(42);
    delete ptr;
    delete ptr;  // Error: double free
    return 0;
}
```

## Интеграция с парсером

Семантический анализ будет вызываться после успешного парсинга:

```c
// В main.c
ASTNode* ast = parser_parse(parser);
if (!parser_has_error(parser)) {
    SemanticContext* semantic = semantic_create();
    bool semantic_ok = semantic_analyze(semantic, ast);
    if (semantic_ok) {
        printf("Semantic analysis passed!\n");
        // Переход к кодогенерации
    } else {
        printf("Semantic errors found\n");
    }
    semantic_destroy(semantic);
}
```

## Ожидаемый результат

После завершения Step 3:
- ✅ Полная проверка типов
- ✅ Валидация областей видимости  
- ✅ Анализ функций
- ✅ Базовая проверка управления памятью
- ✅ Подробные сообщения об ошибках
- ✅ Comprehensive test suite

Это подготовит нас к Step 4 (Code Generation), где мы будем транспилировать валидный AST в C код. 