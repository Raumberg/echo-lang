// Пример работы с указателями
#include core::mem
#include core::io

fn main() -> i32 {
    // Стековые переменные
    i32 x = 42;
    i32* ptr = &x;
    
    io::print("Value: " + *ptr);
    
    // Динамическая память
    i32* heap_int = alloc i32(100);
    *heap_int = 200;
    
    io::print("Heap value: " + *heap_int);
    delete heap_int;
    
    // Массивы
    i32* arr = alloc i32[5];
    for (i32 i = 0; i < 5; i++) {
        arr[i] = i * 10;
    }
    
    for (i32 i = 0; i < 5; i++) {
        io::print("arr[" + i + "] = " + arr[i]);
    }
    
    delete[] arr;
    return 0;
} 