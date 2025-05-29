# Generics and Auto Type Rules in Echo

## Overview

Echo язык поддерживает систему generics через ключевое слово `auto`, но есть четкие ограничения где и как его можно использовать.

## Valid Uses of `auto`

### ✅ 1. Variable Declarations with Initialization
Переменные с инициализацией могут использовать `auto` - тип выводится из значения:

```echo
fn main() -> void {
    auto number = 42;           // i32
    auto pi = 3.14;            // f64
    auto message = "Hello";     // string
    auto flag = true;          // bool
}
```

### ✅ 2. Generic Function Parameters
Параметры функций могут быть `auto` - создают generic функции:

```echo
fn add(auto a, auto b) -> auto {
    return a + b;
}

fn identity(auto x) -> auto {
    return x;
}
```

### ✅ 3. Generic Function Return Types
Возвращаемый тип может быть `auto` если выводится из контекста:

```echo
fn max(auto a, auto b) -> auto {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
```

### ✅ 4. Local Variables in Generic Functions
Внутри generic функций можно использовать `auto`:

```echo
fn process(auto input) -> auto {
    auto temp = input * 2;     // Тип выводится из input
    auto result = temp + 1;
    return result;
}
```

## Invalid Uses of `auto`

### ❌ 1. Struct Fields
Поля структур **НЕ МОГУТ** быть `auto`:

```echo
// ❌ НЕПРАВИЛЬНО - компилируется с ошибкой
struct Person {
    string name;
    auto age;        // ERROR: Auto не разрешен в полях структур
}

// ✅ ПРАВИЛЬНО
struct Person {
    string name;
    i32 age;         // Используйте конкретный тип
}
```

**Причина**: Структуры должны иметь фиксированную компоновку в памяти. C требует знать размер всех полей на этапе компиляции.

### ❌ 2. Global Variables without Initialization
```echo
// ❌ НЕПРАВИЛЬНО
auto global_var;  // ERROR: Не могу вывести тип

// ✅ ПРАВИЛЬНО  
auto global_var = 100;  // OK: тип выводится как i32
i32 global_var = 100;   // OK: явный тип
```

### ❌ 3. Function Parameters without Generic Intent
Если функция не должна быть generic, используйте конкретные типы:

```echo
// ❌ Делает функцию generic без необходимости
fn print_number(auto num) -> void {
    io::print_int(num);
}

// ✅ ЛУЧШЕ - конкретный тип
fn print_number(i32 num) -> void {
    io::print_int(num);
}
```

## Type Inference Rules

### 1. Literal Type Inference
- `42` → `i32`
- `3.14` → `f64`  
- `"hello"` → `string`
- `true`/`false` → `bool`
- `'c'` → `char`

### 2. Expression Type Inference
```echo
auto a = 10;           // i32
auto b = 20;           // i32  
auto sum = a + b;      // i32 (оба операнда i32)

auto x = 1.5;          // f64
auto y = 2.0;          // f64
auto result = x * y;   // f64 (оба операнда f64)
```

### 3. Struct Literal Type Inference
```echo
struct Point {
    f64 x;
    f64 y;
}

fn main() -> void {
    auto point = Point {x: 1.0, y: 2.0};  // Point (выводится из литерала)
}
```

## Generic Function Instantiation

Когда вызывается generic функция, компилятор создает конкретную версию:

```echo
fn add(auto a, auto b) -> auto {
    return a + b;
}

fn main() -> void {
    auto result1 = add(10, 20);        // Создается add_i32_i32
    auto result2 = add(1.5, 2.5);      // Создается add_f64_f64
}
```

Каждая уникальная комбинация типов создает отдельную функцию в C коде.

## Error Messages

### Struct Field Auto Error
```
error: Auto type is not allowed in struct field 'age'. 
       Struct fields must have concrete types
  --> test.ec:5:10
   |
5  |     auto age;
   |     ^^^^^^^^^ auto not allowed here
```

### Missing Initializer Error  
```
error: Auto variable 'x' must have an initializer
  --> test.ec:3:5
   |
3  |     auto x;
   |     ^^^^^^ cannot infer type without value
```

## Best Practices

### 1. Используйте auto разумно
- ✅ Для переменных где тип очевиден: `auto count = items.length()`
- ✅ Для generic функций где нужна гибкость типов
- ❌ Не злоупотребляйте - иногда явный тип читается лучше

### 2. Структуры всегда конкретные
```echo
// ✅ Хорошо - явные типы
struct Config {
    string filename;
    i32 timeout;
    bool debug_mode;
}
```

### 3. Generic функции для алгоритмов
```echo
// ✅ Хорошо использовать auto для универсальных алгоритмов
fn swap(auto& a, auto& b) -> void {
    auto temp = a;
    a = b;
    b = temp;
}
```

## Future Extensions

В будущих версиях планируется добавить:
- Template структуры: `struct Vector<T> { T* data; i32 size; }`
- Constraints: `fn sort(auto arr) where T: Comparable`
- Type aliases: `type MyInt = i32;`

---

**Запомните**: `auto` в Echo это инструмент для вывода типов и создания generic кода, но со строгими ограничениями для поддержания производительности и совместимости с C. 