#include core::io

// Пример 3: Основы структур
// Демонстрирует: определение структур, инициализацию, доступ к полям

struct Point {
    f64 x;
    f64 y;
}

struct Person {
    string name;
    i32 age;
    bool is_employed;
    Point location;
}

fn main() -> void {
    io::print("=== Structs Basics Demo ===");
    
    // Создание простой структуры
    auto origin = Point {x: 0.0, y: 0.0};
    auto destination = Point {x: 10.5, y: 20.7};
    
    io::print("Points created:");
    io::print("Origin and destination initialized");
    
    // Создание вложенной структуры
    auto person = Person {
        name: "Bob",
        age: 30,
        is_employed: true,
        location: Point {x: 100.0, y: 200.0}
    };
    
    // Доступ к полям
    io::print("");
    io::print("Person info:");
    io::print(person.name);
    io::print_int(person.age);
    io::print_bool(person.is_employed);
    
    // Доступ к вложенным полям
    io::print("");
    io::print("Person location:");
    io::print("X and Y coordinates accessed");
    
    // Изменение полей
    person.age = 31;
    person.is_employed = false;
    
    io::print("");
    io::print("After updates:");
    io::print_int(person.age);
    io::print_bool(person.is_employed);
} 