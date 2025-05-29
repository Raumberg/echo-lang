#include core::io

// Пример 5: Простые Generic функции
// Демонстрирует: auto параметры, полиморфизм, type inference

// Простейшая generic функция
fn identity(auto x) -> auto {
    return x;
}

// Generic функция с двумя параметрами
fn add(auto a, auto b) -> auto {
    return a + b;
}

// Generic функция с условной логикой
fn max(auto a, auto b) -> auto {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// Generic функция с локальными вычислениями
fn square_and_double(auto x) -> auto {
    auto squared = x * x;
    auto result = squared * 2;
    return result;
}

fn main() -> void {
    io::print("=== Simple Generics Demo ===");
    
    // Тестирование identity с разными типами
    io::print("Identity function:");
    auto int_id = identity(42);
    auto float_id = identity(3.14);
    auto string_id = identity("Hello");
    auto bool_id = identity(true);
    
    io::print_int(int_id);      // 42
    io::print(string_id);       // Hello
    io::print_bool(bool_id);    // true
    
    // Тестирование add с разными типами
    io::print("");
    io::print("Add function:");
    auto sum_int = add(10, 20);
    auto sum_float = add(1.5, 2.7);
    
    io::print_int(sum_int);     // 30
    
    // Тестирование max
    io::print("");
    io::print("Max function:");
    auto max_int = max(15, 25);
    auto max_float = max(3.14, 2.71);
    
    io::print_int(max_int);     // 25
    
    // Тестирование square_and_double
    io::print("");
    io::print("Square and double:");
    auto result_int = square_and_double(5);      // (5*5)*2 = 50
    auto result_float = square_and_double(2.0);  // (2*2)*2 = 8
    
    io::print_int(result_int);  // 50
    
    // Демонстрация автовывода в переменных
    io::print("");
    io::print("Auto inference in action:");
    auto x = 100;
    auto y = 200;
    auto combined = add(x, y);
    auto bigger = max(combined, 500);
    
    io::print_int(bigger);      // 500
    
    io::print("");
    io::print("Generics allow code reuse across types!");
} 