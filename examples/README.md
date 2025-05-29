# Echo Language Examples

Добро пожаловать в коллекцию примеров Echo Language! Эти примеры демонстрируют возможности современного системного языка программирования с C-подобным синтаксисом, статической типизацией и мощной системой generics.

## 📚 Примеры по уровню сложности

### 🌟 Уровень 1: Основы

#### [01_hello_world.ec](01_hello_world.ec)
**Что изучается:** Базовый синтаксис, импорты, функции
```echo
#include core::io

fn main() -> void {
    io::print("Hello, Echo Language!");
}
```
- Простейший пример программы
- Импорт модуля `core::io`
- Функция `main` как точка входа

#### [02_basic_types.ec](02_basic_types.ec)
**Что изучается:** Типы данных, автовывод типов, арифметические операции
```echo
auto year = 2024;           // i32
auto pi = 3.14159;          // f64
auto greeting = "Hello";    // string
auto active = false;        // bool
```
- Явные типы vs автовывод с `auto`
- Базовые типы: `i32`, `f64`, `string`, `bool`
- Арифметические вычисления

### 🌟 Уровень 2: Структуры и управление

#### [03_structs_basics.ec](03_structs_basics.ec)
**Что изучается:** Определение структур, инициализация, доступ к полям
```echo
struct Person {
    string name;
    i32 age;
    Point location;
}

auto person = Person {
    name: "Bob",
    age: 30,
    location: Point {x: 100.0, y: 200.0}
};
```
- Определение пользовательских типов
- Вложенные структуры
- Доступ к полям через `.`

#### [04_control_flow.ec](04_control_flow.ec)
**Что изучается:** Условия, циклы, вложенные конструкции
```echo
if (temperature > 20) {
    io::print("It's warm outside!");
}

for (i32 i = 1; i <= 5; i = i + 1) {
    io::print_int(i);
}

while (counter > 0) {
    counter = counter - 1;
}
```
- Условные операторы `if/else`
- Циклы `for` и `while`
- Логические операции и сравнения

### 🌟 Уровень 3: Generics и модули

#### [05_simple_generics.ec](05_simple_generics.ec)
**Что изучается:** Generic функции, полиморфизм, type inference
```echo
fn identity(auto x) -> auto {
    return x;
}

fn add(auto a, auto b) -> auto {
    return a + b;
}

auto int_result = add(10, 20);      // Работает с int
auto float_result = add(1.5, 2.5); // Работает с float
```
- Параметры `auto` создают generic функции
- Одна функция работает с разными типами
- Автоматическая инстанциация для каждого типа

#### [06_modules_imports.ec](06_modules_imports.ec)
**Что изучается:** Модульная система, импорты, aliases
```echo
#include core::io
#include core::io::print as display
#include core::io::print_int as show_number

io::print("Full qualified name");
display("Aliased function");
show_number(42);
```
- Разные способы импорта функций
- Aliases для удобства
- Namespace organization

### 🌟 Уровень 4: Продвинутые структуры

#### [07_data_structures.ec](07_data_structures.ec)
**Что изучается:** Сложные вложенные структуры, helper функции
```echo
struct Material {
    string name;
    f64 density;
    bool is_transparent;
    Color color;
}

struct Object3D {
    string name;
    Dimensions size;
    Material material;
    f64 mass;
}
```
- Многоуровневые вложенные структуры
- Функции-конструкторы
- Организация сложных данных

#### [08_generic_algorithms.ec](08_generic_algorithms.ec)
**Что изучается:** Сложные generic алгоритмы, рекурсия
```echo
fn min3(auto a, auto b, auto c) -> auto {
    auto min_ab = (a < b) ? a : b;
    return (c < min_ab) ? c : min_ab;
}

fn factorial(auto n) -> auto {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
```
- Универсальные алгоритмы
- Рекурсивные функции
- Полиморфизм в действии

### 🌟 Уровень 5: Реальные приложения

#### [09_calculator.ec](09_calculator.ec)
**Что изучается:** Практическое применение, состояние, обработка ошибок
```echo
struct CalculatorState {
    f64 current_value;
    f64 previous_value;
    string last_operation;
    bool has_error;
}

fn calculate(auto a, auto b, string operation) -> auto {
    if (operation == "divide" && b == 0) {
        return 0;  // Error handling
    }
    // ... операции
}
```
- Управление состоянием
- Обработка ошибок (деление на ноль)
- Структурированное программирование
- Generic функции в реальном применении

#### [10_advanced_showcase.ec](10_advanced_showcase.ec)
**Что изучается:** Все концепции языка в комплексном приложении
```echo
// Игровой движок в миниатюре
struct GameWorld {
    string name;
    i32 entity_count;
    Entity player;
    Entity enemy;
    Entity powerup;
}

fn simulate_game_loop(GameWorld world, i32 frame_count) -> GameWorld {
    // Симуляция игрового цикла с коллизиями,
    // эффектами и обновлением состояния
}
```
- Комплексная архитектура приложения
- Симуляция игрового движка
- Математические вычисления
- Система эффектов и анимация

## 🚀 Как запускать примеры

### Компиляция отдельного примера:
```bash
# Сборка компилятора
make all

# Компиляция примера
./bin/echo examples/01_hello_world.ec

# Сборка C кода
gcc -o program examples/01_hello_world.c examples/echo_runtime.c

# Запуск
./program
```

### Тестирование всех примеров:
```bash
# Запуск всех примеров последовательно
for example in examples/*.ec; do
    echo "=== Testing $example ==="
    ./bin/echo "$example"
    echo ""
done
```

## 🎯 Ключевые концепции Echo

### 1. **Статическая типизация с выводом типов**
```echo
i32 explicit_type = 42;    // Явный тип
auto inferred_type = 42;   // Автовывод -> i32
```

### 2. **Мощная система Generics**
```echo
fn sort(auto arr) -> auto {  // Универсальная сортировка
    // Работает с любым сравнимым типом
}
```

### 3. **Безопасность и производительность**
- Компиляция в эффективный C код
- Статическая проверка типов
- RAII управление памятью
- Отсутствие runtime overhead

### 4. **Модульная архитектура**
```echo
#include core::io::print as log     // Функциональный импорт
#include core::mem as memory        // Модульный импорт
```

### 5. **C-совместимость**
- Прямая генерация C кода
- Интеграция с существующими библиотеками
- Предсказуемая производительность

## 📖 Progression Path

**Рекомендуемый порядок изучения:**

1. **Базы (01-02)**: Синтаксис и типы
2. **Структуры (03-04)**: Организация данных и управление
3. **Generics (05-06)**: Полиморфизм и модули
4. **Продвинутое (07-08)**: Сложные структуры и алгоритмы
5. **Практика (09-10)**: Реальные приложения

## 🔧 Возможности языка

### ✅ Что работает отлично:
- ✅ Статическая типизация с автовыводом
- ✅ Generic функции через `auto`
- ✅ Структуры с конкретными типами
- ✅ Модульная система с импортами
- ✅ C-кодогенерация
- ✅ Управляющие конструкции
- ✅ Функции высшего порядка (через generics)

### 🚧 Ограничения (by design):
- ❌ `auto` в полях структур (нарушает C-совместимость)
- ❌ Runtime reflection (статическая компиляция)
- ❌ Garbage collector (RAII управление памятью)

## 🎓 Learning Resources

- **Документация**: `docs/generics_rules.md` - правила использования `auto`
- **Design Philosophy**: `design.md` - принципы языка
- **Changelog**: `CHANGELOG.md` - история развития

---

**Echo Language** - это современный системный язык, сочетающий выразительность Rust с простотой и производительностью C. Изучайте примеры последовательно и создавайте потрясающие приложения! 🚀 