// Hello World в Echo
#include core::io

fn main() -> i32 {
    string message = "Hello, Echo!";
    io::print(message);
    
    // Простая арифметика
    i32 a = 10;
    i32 b = 20;
    i32 sum = add(a, b);
    
    io::print("Sum: " + sum);
    
    return 0;
}

fn add(i32 x, i32 y) -> i32 {
    return x + y;
} 