// Демонстрация управления памятью в Echo
#include core::mem
#include core::io

struct Point {
    f64 x;
    f64 y;
}

fn main() -> i32 {
    // Стековые объекты - автоматическое освобождение
    i32 stack_var = 42;
    Point stack_point = {3.14, 2.71};
    
    // Динамическая память с alloc
    i32* heap_int = alloc i32(100);
    *heap_int = 200;
    
    Point* heap_point = alloc Point{1.0, 2.0};
    heap_point.x = 5.0;
    
    // Массивы в куче
    i32* numbers = alloc i32[10];
    for (i32 i = 0; i < 10; i++) {
        numbers[i] = i * i;
    }
    
    // Строки
    string* message = alloc string("Hello from heap!");
    
    // Вывод результатов
    io::print("Stack var: " + stack_var);
    io::print("Heap int: " + *heap_int);
    io::print("Point: (" + heap_point.x + ", " + heap_point.y + ")");
    io::print("Message: " + *message);
    
    // Освобождение памяти
    delete heap_int;
    delete heap_point;
    delete[] numbers;
    delete message;
    
    return 0;
} 