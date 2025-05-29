#include core::io

// Пример 4: Управляющие конструкции
// Демонстрирует: if/else, циклы for/while, вложенные конструкции

fn check_number(i32 num) -> void {
    if (num > 0) {
        io::print("Positive number");
    } else {
        if (num < 0) {
            io::print("Negative number");
        } else {
            io::print("Zero");
        }
    }
}

fn main() -> void {
    io::print("=== Control Flow Demo ===");
    
    // Простые условия
    i32 temperature = 25;
    if (temperature > 20) {
        io::print("It's warm outside!");
    } else {
        io::print("It's cold outside!");
    }
    
    // Проверка чисел
    io::print("");
    io::print("Number checks:");
    check_number(5);
    check_number(-3);
    check_number(0);
    
    // Цикл for
    io::print("");
    io::print("Counting up:");
    for (i32 i = 1; i <= 5; i = i + 1) {
        io::print_int(i);
    }
    
    // Цикл while
    io::print("");
    io::print("Counting down:");
    i32 counter = 5;
    while (counter > 0) {
        io::print_int(counter);
        counter = counter - 1;
    }
    
    // Вложенные циклы (используем разные имена переменных)
    io::print("");
    io::print("Multiplication table (3x3):");
    for (i32 row = 1; row <= 3; row = row + 1) {
        for (i32 col = 1; col <= 3; col = col + 1) {
            auto product = row * col;
            io::print_int(product);
        }
    }
    
    // Условия с boolean переменными
    bool is_weekend = true;
    bool is_sunny = false;
    
    if (is_weekend && is_sunny) {
        io::print("Perfect day for a picnic!");
    } else {
        if (is_weekend) {
            io::print("Weekend, but weather could be better");
        } else {
            io::print("Workday - stay focused!");
        }
    }
} 