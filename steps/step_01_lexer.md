# Шаг 1: Лексер (Tokenizer)

## Цель
Создать лексер, который разбивает исходный код Echo на токены.

## Токены для поддержки

### Ключевые слова
```
fn, struct, enum, if, else, for, while, return, break, continue,
auto, null, true, false, alloc, delete, sizeof,
const, static, global, typedef, switch, case, default
```

### Типы данных
```
i8, i16, i32, i64, f32, f64, bool, string, char, void
```

### Операторы
```
+, -, *, /, %, =, ==, !=, <, >, <=, >=, 
&&, ||, !, &, |, ^, <<, >>, ++, --, 
+=, -=, *=, /=, %=, ->, ?, ::
```

### Разделители
```
(, ), {, }, [, ], ;, ,, :, ., #
```

### Литералы
```
123 (integer)
3.14 (float)
"hello" (string)
'c' (char)
true/false (boolean)
null (null literal)
```

### Идентификаторы
```
variable_name, function_name, Type_Name
```

### Комментарии
```
// однострочный комментарий
/* многострочный комментарий */
```

## Структура токена

```c
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
    int length;
} Token;
```

## Реализация

### Файлы для создания:
1. `src/lexer.h` - заголовочный файл
2. `src/lexer.c` - реализация лексера
3. `tests/test_lexer.c` - тесты

### Основные функции:
```c
// Инициализация лексера
Lexer* lexer_create(const char* source_code);

// Получение следующего токена
Token lexer_next_token(Lexer* lexer);

// Освобождение памяти
void lexer_destroy(Lexer* lexer);
void token_destroy(Token* token);

// Утилиты
bool is_keyword(const char* str);
bool is_operator(char c);
bool is_delimiter(char c);
```

## Примеры токенизации

### Входной код:
```echo
fn add(a: i32, b: i32) -> i32 {
    return a + b
}
```

### Ожидаемые токены:
```
TOKEN_KEYWORD: "fn"
TOKEN_IDENTIFIER: "add"
TOKEN_DELIMITER: "("
TOKEN_IDENTIFIER: "a"
TOKEN_DELIMITER: ":"
TOKEN_KEYWORD: "i32"
TOKEN_DELIMITER: ","
TOKEN_IDENTIFIER: "b"
TOKEN_DELIMITER: ":"
TOKEN_KEYWORD: "i32"
TOKEN_DELIMITER: ")"
TOKEN_OPERATOR: "->"
TOKEN_KEYWORD: "i32"
TOKEN_DELIMITER: "{"
TOKEN_KEYWORD: "return"
TOKEN_IDENTIFIER: "a"
TOKEN_OPERATOR: "+"
TOKEN_IDENTIFIER: "b"
TOKEN_DELIMITER: "}"
TOKEN_EOF
```

## Тесты для написания

1. **Базовые токены** - ключевые слова, операторы, разделители
2. **Литералы** - числа, строки, символы
3. **Идентификаторы** - имена переменных и функций
4. **Комментарии** - однострочные и многострочные
5. **Ошибки** - некорректные символы, незакрытые строки
6. **Позиция** - правильное отслеживание строк и столбцов

## Критерии готовности

- [x] Лексер корректно разбирает все базовые токены
- [x] Правильно обрабатывает строки с escape-последовательностями
- [x] Игнорирует комментарии
- [x] Отслеживает позицию в файле (строка, столбец)
- [x] Обрабатывает ошибки (неизвестные символы)
- [x] Все тесты проходят
- [x] Нет утечек памяти

## Время выполнения
**Оценка: 3-5 дней** ✅ **ЗАВЕРШЕНО!**

## Следующий шаг
После завершения лексера переходим к **Шагу 2: Парсер (AST)** 