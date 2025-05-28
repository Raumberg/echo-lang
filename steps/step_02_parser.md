# Шаг 2: Парсер (AST)

## Цель
Создать парсер, который строит абстрактное синтаксическое дерево (AST) из токенов лексера.

## AST узлы для поддержки

### Базовые узлы
```c
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VARIABLE_DECL,
    AST_PARAMETER,
    AST_BLOCK,
    AST_RETURN,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_IDENTIFIER,
    AST_LITERAL,
    AST_TYPE,
    AST_STRUCT,
    AST_ENUM,
    AST_ASSIGNMENT,
    AST_ARRAY_ACCESS,
    AST_MEMBER_ACCESS,
    AST_POINTER_DEREF,
    AST_ADDRESS_OF,
    AST_ALLOC,
    AST_DELETE,
    AST_PREPROCESSOR
} ASTNodeType;
```

### Структура узла AST
```c
typedef struct ASTNode {
    ASTNodeType type;
    char* value;
    struct ASTNode** children;
    int child_count;
    int line;
    int column;
    
    // Type information
    char* data_type;
    bool is_pointer;
    bool is_optional;
} ASTNode;
```

## Грамматика Echo (упрощенная)

### Программа
```
program := (function | struct | enum | preprocessor)*

function := 'fn' IDENTIFIER '(' parameter_list? ')' '->' type block
parameter_list := parameter (',' parameter)*
parameter := type IDENTIFIER

struct := 'struct' IDENTIFIER '{' field_list '}'
field_list := field (',' field)*
field := type IDENTIFIER

enum := 'enum' IDENTIFIER '{' enum_list '}'
enum_list := IDENTIFIER (',' IDENTIFIER)*
```

### Выражения
```
expression := assignment
assignment := logical_or ('=' assignment)?
logical_or := logical_and ('||' logical_and)*
logical_and := equality ('&&' equality)*
equality := comparison (('==' | '!=') comparison)*
comparison := term (('<' | '>' | '<=' | '>=') term)*
term := factor (('+' | '-') factor)*
factor := unary (('*' | '/' | '%') unary)*
unary := ('!' | '-' | '+' | '*' | '&' | '++' | '--')? primary
primary := IDENTIFIER | LITERAL | '(' expression ')' | call | array_access | member_access
```

### Операторы
```
statement := return_stmt | if_stmt | for_stmt | while_stmt | 
            variable_decl | expression_stmt | block

return_stmt := 'return' expression? ';'
if_stmt := 'if' '(' expression ')' statement ('else' statement)?
for_stmt := 'for' '(' variable_decl expression ';' expression ')' statement
while_stmt := 'while' '(' expression ')' statement
variable_decl := type IDENTIFIER ('=' expression)? ';'
expression_stmt := expression ';'
block := '{' statement* '}'
```

## Реализация

### Файлы для создания:
1. `src/ast.h` - определения AST узлов
2. `src/ast.c` - функции для работы с AST
3. `src/parser.h` - заголовочный файл парсера
4. `src/parser.c` - реализация парсера
5. `tests/test_parser.c` - тесты парсера

### Основные функции парсера:
```c
// Создание и уничтожение парсера
Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);

// Основная функция парсинга
ASTNode* parser_parse(Parser* parser);

// Парсинг различных конструкций
ASTNode* parse_program(Parser* parser);
ASTNode* parse_function(Parser* parser);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_type(Parser* parser);

// Утилиты
Token parser_current_token(Parser* parser);
Token parser_peek_token(Parser* parser);
void parser_advance(Parser* parser);
bool parser_match(Parser* parser, TokenType type);
bool parser_expect(Parser* parser, TokenType type, const char* message);
```

### Функции для работы с AST:
```c
// Создание узлов
ASTNode* ast_create_node(ASTNodeType type, const char* value);
ASTNode* ast_create_binary_op(const char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_create_unary_op(const char* op, ASTNode* operand);
ASTNode* ast_create_function(const char* name, ASTNode* params, ASTNode* return_type, ASTNode* body);

// Управление детьми
void ast_add_child(ASTNode* parent, ASTNode* child);
void ast_destroy(ASTNode* node);

// Утилиты
void ast_print(ASTNode* node, int indent);
ASTNode* ast_find_function(ASTNode* program, const char* name);
```

## Примеры парсинга

### Входной код:
```echo
fn add(i32 a, i32 b) -> i32 {
    return a + b
}
```

### Ожидаемое AST:
```
PROGRAM
└── FUNCTION "add"
    ├── PARAMETER
    │   ├── TYPE "i32"
    │   └── IDENTIFIER "a"
    ├── PARAMETER
    │   ├── TYPE "i32"
    │   └── IDENTIFIER "b"
    ├── TYPE "i32" (return type)
    └── BLOCK
        └── RETURN
            └── BINARY_OP "+"
                ├── IDENTIFIER "a"
                └── IDENTIFIER "b"
```

## Обработка ошибок

### Типы ошибок:
1. **Синтаксические ошибки** - неожиданные токены
2. **Незакрытые блоки** - отсутствующие `}`
3. **Неправильные выражения** - некорректный синтаксис
4. **Отсутствующие токены** - ожидаемые `;`, `,`, etc.

### Стратегия восстановления:
1. **Panic mode** - пропускать токены до синхронизирующего
2. **Синхронизирующие токены** - `;`, `}`, `fn`, `struct`
3. **Максимальное количество ошибок** - остановка после 10 ошибок

## Тесты для написания

1. **Простые выражения** - `2 + 3`, `a * b`
2. **Функции** - объявление и вызов
3. **Переменные** - объявление и присваивание
4. **Условия** - if/else
5. **Циклы** - for, while
6. **Указатели** - `*ptr`, `&var`, `new`, `delete`
7. **Структуры** - объявление и использование
8. **Ошибки** - некорректный синтаксис

## Критерии готовности

- [ ] Парсер строит корректное AST для базовых конструкций
- [ ] Правильно обрабатывает приоритет операторов
- [ ] Поддерживает все типы выражений
- [ ] Обрабатывает функции с параметрами
- [ ] Парсит структуры и enum'ы
- [ ] Корректно обрабатывает ошибки
- [ ] Все тесты проходят
- [ ] AST можно красиво вывести для отладки

## Время выполнения
**Оценка: 5-7 дней**

## Следующий шаг
После завершения парсера переходим к **Шагу 3: Семантический анализ** 