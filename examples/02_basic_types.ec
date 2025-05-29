#include core::io

// Пример 2: Базовые типы и автовывод
// Демонстрирует: типы данных, auto inference, арифметику

fn main() -> void {
    io::print("=== Basic Types Demo ===");
    
    // Явные типы
    i32 age = 25;
    f64 height = 175.5;
    bool is_student = true;
    string name = "Alice";
    
    // Автовывод типов
    auto year = 2024;           // i32
    auto pi = 3.14159;          // f64
    auto greeting = "Hello";    // string
    auto active = false;        // bool
    
    // Вывод значений
    io::print("Explicit types:");
    io::print(name);
    io::print_int(age);
    io::print_bool(is_student);
    
    io::print("");
    io::print("Auto-inferred types:");
    io::print_int(year);
    io::print(greeting);
    io::print_bool(active);
    
    // Арифметические операции
    auto sum = age + year;
    auto product = height * 2.0;
    
    io::print("");
    io::print("Calculations:");
    io::print_int(sum);
} 