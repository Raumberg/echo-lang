#include core::io
#include core::io::print as display
#include core::io::print_int as show_number

// Пример 6: Модульная система и импорты
// Демонстрирует: различные способы импорта, aliases, qualified names

fn demonstrate_imports() -> void {
    // Использование полных qualified names
    io::print("Using full qualified names:");
    io::print_int(42);
    io::print_bool(true);
    
    // Использование aliases
    display("Using function alias:");
    show_number(100);
    
    // Смешанное использование
    io::print("Mixed usage works perfectly!");
}

fn math_operations() -> void {
    auto a = 10;
    auto b = 20;
    auto sum = a + b;
    
    io::print("Math results:");
    show_number(sum);  // Используем alias
    
    if (sum > 25) {
        display("Sum is greater than 25");  // Используем другой alias
    }
}

fn main() -> void {
    io::print("=== Modules and Imports Demo ===");
    io::print("");
    
    // Демонстрация различных способов вызова
    io::print("Direct io::print call");
    display("Aliased print call");
    show_number(999);
    
    io::print("");
    demonstrate_imports();
    
    io::print("");
    math_operations();
    
    io::print("");
    io::print("Module system provides:");
    display("- Namespace organization");
    display("- Function aliases");
    display("- Clean code structure");
    
    io::print("");
    io::print("All import styles work seamlessly together!");
} 