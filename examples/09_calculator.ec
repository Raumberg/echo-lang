#include core::io

// Пример 9: Простой калькулятор (упрощенная версия)
// Демонстрирует: практическое применение, generic функции, обработка ошибок

// Generic функция для математических операций
fn add(auto a, auto b) -> auto {
    return a + b;
}

fn subtract(auto a, auto b) -> auto {
    return a - b;
}

fn multiply(auto a, auto b) -> auto {
    return a * b;
}

fn divide(auto a, auto b) -> auto {
    if (b != 0) {
        return a / b;
    } else {
        return 0;  // Error case - division by zero
    }
}

// Проверка на деление на ноль
fn is_zero(auto x) -> bool {
    return x == 0;
}

// Возведение в квадрат
fn square(auto x) -> auto {
    return x * x;
}

// Абсолютное значение
fn abs_value(auto x) -> auto {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

// Вычисление процента
fn percentage(auto value, auto percent) -> auto {
    return (value * percent) / 100;
}

// Простое возведение в степень
fn power(auto base, auto exp) -> auto {
    if (exp == 0) {
        return 1;
    }
    
    auto result = base;
    for (i32 i = 1; i < exp; i = i + 1) {
        result = result * base;
    }
    
    return result;
}

// Факториал (только для небольших чисел)
fn factorial(auto n) -> auto {
    if (n <= 1) {
        return 1;
    } else {
        auto prev = factorial(n - 1);
        return n * prev;
    }
}

fn main() -> void {
    io::print("=== Simple Calculator Demo ===");
    
    // Базовые арифметические операции
    io::print("Basic arithmetic:");
    auto sum = add(15, 25);
    auto diff = subtract(50, 20);
    auto product = multiply(7, 8);
    auto quotient = divide(100, 4);
    
    io::print_int(sum);        // 40
    io::print_int(diff);       // 30
    io::print_int(product);    // 56
    io::print_int(quotient);   // 25
    
    // Работа с дробными числами
    io::print("");
    io::print("Float calculations:");
    auto float_sum = add(3.14, 2.86);
    auto float_product = multiply(2.5, 4.0);
    
    io::print("Float operations completed");
    
    // Проверка деления на ноль
    io::print("");
    io::print("Division by zero test:");
    auto zero_result = divide(10, 0);
    if (is_zero(zero_result)) {
        io::print("Division by zero handled correctly");
    }
    
    // Продвинутые функции
    io::print("");
    io::print("Advanced functions:");
    auto squared = square(8);
    auto abs_val = abs_value(-15);
    auto percent_val = percentage(200, 15);
    
    io::print_int(squared);    // 64
    io::print_int(abs_val);    // 15
    io::print_int(percent_val); // 30
    
    // Возведение в степень
    io::print("");
    io::print("Power calculations:");
    auto power_2_3 = power(2, 3);
    auto power_5_2 = power(5, 2);
    
    io::print_int(power_2_3);  // 8
    io::print_int(power_5_2);  // 25
    
    // Факториал
    io::print("");
    io::print("Factorial calculations:");
    auto fact_4 = factorial(4);
    auto fact_5 = factorial(5);
    
    io::print_int(fact_4);     // 24
    io::print_int(fact_5);     // 120
    
    // Сложные вычисления
    io::print("");
    io::print("Complex calculations:");
    auto complex1 = add(square(5), multiply(3, 4));  // 25 + 12 = 37
    auto complex2 = subtract(power(3, 3), factorial(3)); // 27 - 6 = 21
    
    io::print_int(complex1);   // 37
    io::print_int(complex2);   // 21
    
    // Демонстрация generic функций с разными типами
    io::print("");
    io::print("Generic type flexibility:");
    auto int_add = add(10, 20);
    auto float_add = add(1.5, 2.5);
    auto mixed_calc = multiply(divide(100, 4), 2);
    
    io::print_int(int_add);    // 30
    io::print("Float addition completed");
    io::print_int(mixed_calc); // 50
    
    io::print("");
    io::print("=== Calculator Features ===");
    io::print("✓ Basic arithmetic (+, -, *, /)");
    io::print("✓ Error handling (division by zero)");
    io::print("✓ Advanced functions (square, abs, %)");
    io::print("✓ Power and factorial calculations");
    io::print("✓ Generic functions for type flexibility");
    io::print("✓ Complex expression evaluation");
    
    io::print("");
    io::print("Calculator demo completed successfully!");
} 