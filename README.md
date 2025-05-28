# Echo Programming Language

Современный компилируемый язык программирования с простым синтаксисом, статической типизацией и возможностью встраивания ассемблера.

## ✨ Особенности языка

### 🎯 Простота и ясность
- **Без lifetimes** как в Rust
- **Без template х*йни** как в C++
- **Простые generics** с `auto`
- **Понятный синтаксис** похожий на C

### 🔒 Безопасность
- **Null safety** с опциональными типами `T?`
- **Статическая типизация** с выводом типов
- **RAII** для автоматического управления ресурсами

### ⚡ Производительность
- **Компиляция в нативный код**
- **Нет сборщика мусора** - детерминированное управление памятью
- **Встроенный ассемблер** для критических участков
- **Совместимость с C** библиотеками

### 🛠 Гибкость управления памятью
- **Стековые объекты** - автоматическое освобождение
- **Обычные указатели** - `T*` для полного контроля
- **alloc/delete** - удобная аллокация
- **Умные указатели** - `unique<T>`, `shared<T>` (опционально)

## 📝 Примеры кода

### Hello World
```echo
#include core::io

fn main() -> i32 {
    string message = "Hello, Echo!"
    io::print(message)
    return 0
}
```

### Работа с указателями
```echo
#include core::mem
#include core::io

fn main() -> i32 {
    // Стековые переменные
    i32 x = 42
    i32* ptr = &x
    
    // Динамическая память
    i32* heap_int = alloc i32(100)
    *heap_int = 200
    
    io::print("Value: " + *heap_int)
    delete heap_int
    
    return 0
}
```

### Функции с generics
```echo
fn max(auto a, auto b) -> auto {
    return a > b ? a : b
}

fn find_element(auto element, [auto] array) -> auto? {
    for (auto item : array) {
        if (item == element) {
            return item
        }
    }
    return null
}
```

### Структуры и null safety
```echo
struct Point {
    f64 x,
    f64 y
}

fn distance(Point? p1, Point? p2) -> f64? {
    if (p1 && p2) {
        f64 dx = p1.x - p2.x
        f64 dy = p1.y - p2.y
        return sqrt(dx*dx + dy*dy)
    }
    return null
}
```

## 🔧 Сборка и использование

### Требования
- GCC или Clang
- Make
- Linux/macOS/Windows

### Компиляция
```bash
git clone <repo>
cd echo-lang
make all
```

### Тестирование лексера
```bash
make all

# Тестирование на примере
./bin/echo examples/hello.echo
./bin/echo examples/pointers.echo
```

### Пример вывода лексера
```
Echo Language Lexer v1.0
========================

Tokenizing file: examples/hello.echo
Tokens:
-------
  1. PREPROCESSOR | #include core::io    | Line  2, Col  1
  2. KEYWORD      | fn                   | Line  4, Col  1
  3. IDENTIFIER   | main                 | Line  4, Col  4
  4. DELIMITER    | (                    | Line  4, Col  8
  5. DELIMITER    | )                    | Line  4, Col  9
  6. OPERATOR     | ->                   | Line  4, Col 11
  7. KEYWORD      | i32                  | Line  4, Col 14
  ...
```

## 📚 Документация

- [`design.md`](design.md) - Полная спецификация языка
- [`steps/`](steps/) - Пошаговый план реализации
- [`examples/`](examples/) - Примеры кода на Echo
- [`docs/`](docs/) - Дополнительная документация

## 🧪 Тестирование

Проект включает comprehensive test suite:

```bash
# Все тесты
make test

# Только лексер
make test-lexer

# Очистка
make clean
```

## 🗺 Roadmap

### Фаза 1: Базовая архитектура
- [x] **Лексер** - токенизация исходного кода
- [x] **Парсер** - построение AST
- [x] **Семантический анализ** - проверка типов
- [ ] **C транспилятор** - генерация C кода

### Фаза 2: Продвинутые фичи
- [ ] **LLVM backend** - прямая компиляция
- [ ] **Оптимизации** - улучшение производительности
- [ ] **Стандартная библиотека** - core модули
- [ ] **Package manager** - управление зависимостями

### Фаза 3: Экосистема
- [ ] **IDE поддержка** - LSP сервер
- [ ] **Отладчик** - интеграция с GDB/LLDB
- [ ] **Профилировщик** - анализ производительности
- [ ] **Документация** - полное руководство

## 🤝 Вклад в проект

Проект находится в активной разработке. Приветствуются:

- 🐛 Баг репорты
- 💡 Предложения по улучшению
- 📝 Улучшения документации
- 🧪 Дополнительные тесты
- 🔧 Исправления и новые фичи

## 📄 Лицензия

Apache 2.0 License - см. [LICENSE](LICENSE) файл.

## 🎯 Философия

Echo создается с целью объединить лучшее из разных языков:

- **Простота C** - понятный синтаксис без магии
- **Безопасность Rust** - но без сложности lifetimes
- **Производительность C++** - но без template ада
- **Удобство Go** - простые generics и модули

**Цель**: создать язык, который легко изучить, приятно использовать и который компилируется в быстрый нативный код. 