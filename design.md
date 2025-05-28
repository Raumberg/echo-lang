# Язык программирования Echo

## Фаза 1: Базовая архитектура

## Синтаксис Echo

### Базовые типы
```
// Простые типы
i32 x
string name = "hello"  

// Сложные типы
[i32::5] arr = [1, 2, 3, 4, 5] // массивы
[i32] dynamic_array // динамический массив

// Автоматический тип (generic)
auto x = 42 // Преобразование и аллокация памяти для переменной x как int32

// Функции
fn add(a: i32, b: i32) -> i32 {
    return a + b
}

// Generics с auto - простые и понятные
fn max(auto a, auto b) -> auto {
    return a > b ? a : b
}

fn find_element(auto element, [auto] array) -> auto? {
    for (auto item : array) {
        if (item == element) {
            return item
        }
    }
    return null  // возвращаем null если не найден
}

// Структуры
struct Point {
    f64 x,
    f64 y
}

// Инициализация структур - просто и ясно
Point p = {1.0, 2.0}  // без лишней хуйни
auto p = {1.0, 2.0}   // автовывод типа

// Enum'ы
enum Forest {
    Oak,
    Pine,
    Birch
}

// Null safety - опциональные типы (но проще чем в Rust)
string? nullable_string = null      // может быть null
string non_null_string = "hello"   // не может быть null
i32? maybe_number = null

// Проверка на null - без лишних unwrap и прочей хуйни
if (nullable_string) {  // просто проверяем как в C
    print(nullable_string)
}

// Поинтеры - для тех кто хочет контроль
i32* ptr = &x           // обычный указатель
i32** double_ptr = &ptr // указатель на указатель
*ptr = 42               // разыменование

// Аллокация памяти
#include core::mem

i32* heap_int = mem::alloc(i32) // то же самое что malloc(sizeof(i32))
*heap_int = 42 // или mem::deref(heap_int)
mem::free(heap_int)

// Или через alloc - более удобно
i32* new_int = alloc i32(42)      // аллокация + инициализация
Point* new_point = alloc Point{1.0, 2.0}  // для структур
delete new_int                  // освобождение
delete new_point

// Умные указатели - для ленивых (опционально)
#include core::ptr

ptr::unique<Point> point = alloc Point{1.0, 2.0}  // автоматическое освобождение
ptr::shared<string> shared_str = alloc string("hello")  // подсчет ссылок
// delete не нужен - автоматически освобождается

// Массивы в куче
i32* arr = alloc i32[10]          // массив из 10 элементов
arr[0] = 42
delete[] arr                    // освобождение массива

// Обработка ошибок - без Result хуйни, просто и понятно
fn divide(i32 a, i32 b) -> i32? {
    if (b == 0) {
        return null  // просто возвращаем null при ошибке
    }
    return a / b
}

// Использование - без unwrap и прочего говна
auto result = divide(10, 2)
if (result) {
    print("Result: " + result)
} else {
    print("Error: division by zero")
}

// Встроенный ассемблер
fn fast_multiply(a: i32, b: i32) -> i32 {
    asm {
        mov eax, a
        imul eax, b
        // результат в eax
    }
}

// Циклы
for (i32 i = 0; i < 10; i++) {
    print(i)
}

for (auto item : array) {  // range-based for
    print(item)
}

while (condition) {
    // код
}

// Условия
if (x > 0) {
    print("positive")
} else if (x < 0) {
    print("negative")  
} else {
    print("zero")
}

// Switch/match
switch (value) {
    case 1:
        print("one")
        break
    case 2, 3:  // множественные значения
        print("two or three")
        break
    default:
        print("other")
}

// Константы
const i32 MAX_SIZE = 100
const string VERSION = "1.0.0"

// Глобальные переменные
global i32 counter = 0

// Статические переменные в функциях
fn get_id() -> i32 {
    static i32 id = 0
    return ++id
}

// Typedef для удобства
typedef i32* IntPtr
typedef fn(i32, i32) -> i32 BinaryOp

// Использование typedef
IntPtr ptr = &x
BinaryOp operation = &add

// Объявление макросов и директивы компиляции, как в C, C++
#module math // объявление модуля

#link stdio
#include stdio as io // линковка с СИ модулем и импорт этого модуля как неймспейс io

#include math::complex // инклюд функции/структуры из модуля или инклюд самого модуля

// Условная компиляция
#if __dbg__
    print("Debug mode")
#endif

#ifdef __win__
    // Windows specific code
#else
    // Unix specific code  
#endif

// Макросы
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))

// Строковые литералы с escape последовательностями
string text = "Hello\nWorld\t!"  // \n, \t, \r, \\, \", \'
string raw = r"C:\path\to\file"  // raw string без escape
string multiline = """
    Многострочная
    строка
"""

// Числовые литералы
i32 decimal = 42
i32 hex = 0xFF
i32 binary = 0b1010
i32 octal = 0o755
f64 scientific = 1.23e-4
f64 with_suffix = 3.14f

// Приоритет операторов (от высшего к низшему):
// 1. () [] -> .
// 2. ! ~ ++ -- + - * & (unary) sizeof alloc delete
// 3. * / %
// 4. + -
// 5. << >>
// 6. < <= > >=
// 7. == !=
// 8. &
// 9. ^
// 10. |
// 11. &&
// 12. ||
// 13. ?:
// 14. = += -= *= /= %= <<= >>= &= ^= |=
```

## Особенности языка

### Управление памятью
- **Стековые объекты** - автоматическое освобождение
- **Обычные поинтеры** - `T*` для полного контроля (как в C)
- **alloc/delete** - удобная аллокация (как в C++)
- **malloc/free** - низкоуровневый контроль (как в C)
- **Умные указатели** - `unique<T>`, `shared<T>` для ленивых
- **НЕТ сборщика мусора** - все детерминированно

### Null Safety
- **Опциональные типы**: `T?` может быть null, `T` - не может
- **Простая проверка**: `if (value)` вместо `if value.is_some()`
- **Автоматическое разыменование** в безопасном контексте

### Generics
- **auto** везде где можно вывести тип
- **Без template х*йни** из C++
- **Без lifetime параметров** из Rust

### Обработка ошибок
1. **Опциональные типы** - `T?` возвращает null при ошибке
3. **Исключения** - для критических ошибок (опционально)

### Указатели
```c
// Обычные указатели
i32* ptr = &variable
i32** ptr_to_ptr = &ptr

// Арифметика указателей
ptr++
ptr += 5
i32 value = *(ptr + 2)

// Приведение типов
void* generic_ptr = ptr
i32* typed_ptr = (i32*)generic_ptr

// Функциональные указатели
fn callback(i32 x) -> void { print(x) }
void(*func_ptr)(i32) = &callback
func_ptr(42)
```

## Транспиляция в C - Упрощения для MVP

### Что упрощаем на первом этапе:
1. **Умные указатели** → обычные указатели C + manual free()
2. **Опциональные типы** → union с флагом valid/invalid
3. **auto** → явные типы (анализируем на этапе компиляции)
4. **alloc/delete** → malloc/free

### Пример транспиляции:
```c
// Echo код:
string? name = get_name()
if (name) {
    print(name)
}

// Транспилируется в C:
typedef struct {
    char* value;
    bool is_valid;
} optional_string;

optional_string name = get_name();
if (name.is_valid) {
    print(name.value);
}
```

### Компоненты
1. **Лексер** - токенизация исходного кода
2. **Парсер** - построение AST
3. **Семантический анализатор** - проверка типов
4. **C транспилятор** - генерация C кода (первый этап)
5. **LLVM backend** - генерация IR и компиляция (второй этап)

### Язык реализации: 
C или C++ (в критических местах), Rust там, где его использовать проще

### План создания языка
1. **Этап 1**: Транспиляция в C (быстрый MVP)
2. **Этап 2**: Прямая компиляция через LLVM
3. **Этап 3**: Оптимизации и продвинутые фичи

### Ключевые слова
```
fn, struct, enum, if, else, for, while, return, break, continue,
auto, null, true, false, alloc, delete, sizeof,
const, static, global, typedef, switch, case, default
```

### О памяти

Echo использует RAII (Resource Acquisition Is Initialization) для автоматического управления ресурсами:

```echo
// Стековые объекты - автоматическое освобождение
i32 x = 42
string s = "hello"

// Динамическая память - явное управление
i32* ptr = alloc i32(42)
delete ptr

// Массивы
i32* arr = alloc i32[10]
delete[] arr

// Умные указатели (будущее расширение)
unique<i32> smart_ptr = make_unique<i32>(42)
// автоматическое освобождение при выходе из области видимости
```

### Указатели и ссылки

```echo
i32 x = 42
i32* ptr = &x        // Указатель на x
i32 value = *ptr     // Разыменование

// Динамическая память
i32* heap_ptr = alloc i32(100)
delete heap_ptr

// Массивы
i32* arr = alloc i32[5]
arr[0] = 10
delete[] arr
```