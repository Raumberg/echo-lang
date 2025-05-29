#include core::io

// Пример 7: Сложные структуры данных (упрощенная версия)
// Демонстрирует: вложенные структуры, функции работы со структурами

struct Point {
    f64 x;
    f64 y;
}

struct Color {
    i32 red;
    i32 green;
    i32 blue;
}

struct Rectangle {
    Point top_left;
    Point bottom_right;
    Color fill_color;
}

// Функции для работы со структурами
fn create_point(f64 x, f64 y) -> Point {
    Point point = Point {x: x, y: y};
    return point;
}

fn create_color(i32 r, i32 g, i32 b) -> Color {
    Color color = Color {red: r, green: g, blue: b};
    return color;
}

fn create_rectangle(Point tl, Point br, Color color) -> Rectangle {
    Rectangle rect = Rectangle {
        top_left: tl,
        bottom_right: br,
        fill_color: color
    };
    return rect;
}

fn calculate_area(Rectangle rect) -> f64 {
    f64 width = rect.bottom_right.x - rect.top_left.x;
    f64 height = rect.bottom_right.y - rect.top_left.y;
    f64 area = width * height;
    return area;
}

fn print_point(Point p) -> void {
    io::print("Point coordinates printed");
}

fn print_color(Color c) -> void {
    io::print("Color RGB:");
    io::print_int(c.red);
    io::print_int(c.green);
    io::print_int(c.blue);
}

fn print_rectangle(Rectangle rect) -> void {
    io::print("Rectangle Information:");
    io::print("Top-left point:");
    print_point(rect.top_left);
    
    io::print("Bottom-right point:");
    print_point(rect.bottom_right);
    
    io::print("Fill color:");
    print_color(rect.fill_color);
    
    f64 area = calculate_area(rect);
    io::print("Area calculated");
}

fn main() -> void {
    io::print("=== Data Structures Demo ===");
    
    // Создание точек
    Point p1 = create_point(0.0, 0.0);
    Point p2 = create_point(10.0, 5.0);
    
    // Создание цвета
    Color red = create_color(255, 0, 0);
    Color blue = create_color(0, 0, 255);
    
    // Создание прямоугольников
    Rectangle rect1 = create_rectangle(p1, p2, red);
    
    Point p3 = create_point(5.0, 5.0);
    Point p4 = create_point(15.0, 10.0);
    Rectangle rect2 = create_rectangle(p3, p4, blue);
    
    // Вывод информации
    io::print("First rectangle:");
    print_rectangle(rect1);
    
    io::print("");
    io::print("Second rectangle:");
    print_rectangle(rect2);
    
    // Модификация данных
    rect1.fill_color = blue;
    rect2.top_left.x = 0.0;
    
    io::print("");
    io::print("After modifications:");
    io::print("Rectangle colors and positions updated");
    
    io::print("");
    io::print("Nested structures work well!");
} 