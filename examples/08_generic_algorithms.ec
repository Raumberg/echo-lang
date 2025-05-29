#include core::io

// Пример 8: Generic алгоритмы
// Демонстрирует: сложные generic функции, алгоритмы, реиспользуемый код

// Простой swap алгоритм
fn swap(auto a, auto b) -> auto {
    // В реальном Echo это должно работать с ссылками
    // Здесь демонстрируем концепцию
    auto temp = a;
    a = b;
    b = temp;
    return a;  // Возвращаем измененное значение
}

// Поиск минимума из трех значений
fn min3(auto a, auto b, auto c) -> auto {
    auto min_ab = a;
    if (b < min_ab) {
        min_ab = b;
    }
    
    auto result = min_ab;
    if (c < result) {
        result = c;
    }
    
    return result;
}

// Поиск максимума из трех значений
fn max3(auto a, auto b, auto c) -> auto {
    auto max_ab = a;
    if (b > max_ab) {
        max_ab = b;
    }
    
    auto result = max_ab;
    if (c > result) {
        result = c;
    }
    
    return result;
}

// Вычисление среднего арифметического
fn average(auto a, auto b, auto c) -> auto {
    auto sum = a + b + c;
    auto result = sum / 3;
    return result;
}

// Алгоритм сортировки (упрощенная версия для трех элементов)
fn sort3(auto a, auto b, auto c) -> auto {
    auto min_val = min3(a, b, c);
    auto max_val = max3(a, b, c);
    
    // Средний элемент
    auto middle = a + b + c - min_val - max_val;
    
    return middle;  // Возвращаем средний элемент как демонстрацию
}

// Проверка, является ли число четным (работает с любым числовым типом)
fn is_even(auto num) -> bool {
    auto remainder = num % 2;
    auto result = (remainder == 0);
    return result;
}

// Факториал (рекурсивный алгоритм)
fn factorial(auto n) -> auto {
    if (n <= 1) {
        return 1;
    } else {
        auto prev = factorial(n - 1);
        auto result = n * prev;
        return result;
    }
}

// Числа Фибоначчи
fn fibonacci(auto n) -> auto {
    if (n <= 1) {
        return n;
    } else {
        auto fib1 = fibonacci(n - 1);
        auto fib2 = fibonacci(n - 2);
        auto result = fib1 + fib2;
        return result;
    }
}

// Возведение в степень
fn power(auto base, auto exp) -> auto {
    if (exp == 0) {
        return 1;
    } else {
        auto prev = power(base, exp - 1);
        auto result = base * prev;
        return result;
    }
}

fn main() -> void {
    io::print("=== Generic Algorithms Demo ===");
    
    // Тестирование с целыми числами
    io::print("Integer algorithms:");
    auto min_int = min3(10, 5, 15);
    auto max_int = max3(10, 5, 15);
    auto avg_int = average(10, 20, 30);
    
    io::print_int(min_int);  // 5
    io::print_int(max_int);  // 15
    io::print_int(avg_int);  // 20
    
    // Тестирование с дробными числами
    io::print("");
    io::print("Float algorithms:");
    auto min_float = min3(3.14, 2.71, 1.41);
    auto max_float = max3(3.14, 2.71, 1.41);
    
    io::print("Min and max calculated for floats");
    
    // Проверка четности
    io::print("");
    io::print("Even/odd checks:");
    auto even_check1 = is_even(4);
    auto even_check2 = is_even(7);
    
    io::print_bool(even_check1);  // true
    io::print_bool(even_check2);  // false
    
    // Рекурсивные алгоритмы
    io::print("");
    io::print("Recursive algorithms:");
    auto fact5 = factorial(5);
    auto fib7 = fibonacci(7);
    auto pow_2_3 = power(2, 3);
    
    io::print_int(fact5);    // 120
    io::print_int(fib7);     // 13
    io::print_int(pow_2_3);  // 8
    
    // Сортировка
    io::print("");
    io::print("Sorting:");
    auto middle = sort3(30, 10, 20);
    io::print_int(middle);   // 20
    
    // Демонстрация полиморфизма
    io::print("");
    io::print("Polymorphic usage:");
    auto result1 = min3(100, 200, 150);
    auto result2 = max3(1.1, 2.2, 1.8);
    
    io::print_int(result1);  // 100
    io::print("Max float calculated");
    
    io::print("");
    io::print("Generic algorithms provide code reuse!");
    io::print("Same algorithm works with different types!");
} 