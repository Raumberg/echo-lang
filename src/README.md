# Echo Compiler Source Code Structure

## Модульная организация

```
src/
├── main.c                  # Точка входа компилятора
├── lexer/                  # Лексический анализ
│   ├── lexer.h
│   └── lexer.c
├── parser/                 # Синтаксический анализ
│   ├── parser.h
│   ├── parser.c
│   ├── parser_constructs.c
│   └── parser_expressions.c
├── ast/                    # Abstract Syntax Tree
│   ├── ast.h
│   └── ast.c
├── semantic/               # Семантический анализ (Step 3)
│   ├── semantic.h
│   ├── semantic.c
│   ├── symbol_table.h
│   ├── symbol_table.c
│   ├── type_checker.h
│   ├── type_checker.c
│   ├── scope_manager.h
│   ├── scope_manager.c
│   └── semantic_errors.h
├── codegen/                # Генерация кода (Step 4)
│   ├── codegen.h
│   ├── codegen.c
│   ├── c_transpiler.h
│   └── c_transpiler.c
└── utils/                  # Утилиты
    ├── string_utils.h
    ├── string_utils.c
    ├── memory_utils.h
    └── memory_utils.c
```

## Этапы разработки

- ✅ **Step 1**: Lexer (Лексический анализ)
- ✅ **Step 2**: Parser (Синтаксический анализ) 
- 🔄 **Step 3**: Semantic Analysis (Семантический анализ)
- ⏳ **Step 4**: Code Generation (Генерация кода)
- ⏳ **Step 5**: Standard Library (Стандартная библиотека)

## Сборка

```bash
# Полная сборка
make all

# Тестирование
make test-parser-unit
make test-examples

# Очистка
make clean
```

## Зависимости между модулями

```
main.c
├── lexer/
├── parser/ → lexer/, ast/
├── ast/
├── semantic/ → ast/, parser/
└── codegen/ → ast/, semantic/
```

Каждый модуль инкапсулирует свою функциональность и предоставляет чистый API для других модулей. 