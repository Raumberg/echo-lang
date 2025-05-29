#include core::io
#include core::io::print as log
#include core::io::print_int as show_num

// Пример 10: Продвинутая демонстрация (упрощенная версия)
// Демонстрирует: все ключевые концепции языка в одной программе

// Базовые структуры
struct Vector2D {
    f64 x;
    f64 y;
}

struct Player {
    string name;
    i32 health;
    i32 score;
    Vector2D position;
}

struct Game {
    string title;
    i32 level;
    Player player1;
    Player player2;
}

// Generic математические функции
fn distance(auto x1, auto y1, auto x2, auto y2) -> auto {
    auto dx = x2 - x1;
    auto dy = y2 - y1;
    // Упрощенное расстояние (без квадратного корня)
    auto result = dx * dx + dy * dy;
    return result;
}

fn lerp(auto start, auto end, auto factor) -> auto {
    auto diff = end - start;
    auto offset = diff * factor;
    auto result = start + offset;
    return result;
}

fn clamp(auto value, auto min_val, auto max_val) -> auto {
    if (value < min_val) {
        return min_val;
    }
    if (value > max_val) {
        return max_val;
    }
    return value;
}

fn max(auto a, auto b) -> auto {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

fn min(auto a, auto b) -> auto {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

// Конструкторы
fn create_vector(auto x, auto y) -> Vector2D {
    Vector2D vec = Vector2D {x: x, y: y};
    return vec;
}

fn create_player(string name, i32 health, Vector2D pos) -> Player {
    Player p = Player {
        name: name,
        health: health,
        score: 0,
        position: pos
    };
    return p;
}

// Игровая логика
fn move_player(Player player, Vector2D velocity, auto delta_time) -> Player {
    auto new_x = player.position.x + velocity.x * delta_time;
    auto new_y = player.position.y + velocity.y * delta_time;
    
    player.position.x = new_x;
    player.position.y = new_y;
    
    return player;
}

fn check_collision(Player p1, Player p2, auto collision_radius) -> bool {
    auto dist = distance(p1.position.x, p1.position.y, p2.position.x, p2.position.y);
    return dist < collision_radius;
}

fn apply_damage(Player player, auto damage) -> Player {
    auto new_health = player.health - damage;
    player.health = clamp(new_health, 0, 100);
    return player;
}

fn add_score(Player player, auto points) -> Player {
    player.score = player.score + points;
    return player;
}

// Анализ и отладка
fn analyze_player(Player p) -> void {
    log("=== Player Analysis ===");
    log(p.name);
    show_num(p.health);
    show_num(p.score);
    log("Position analyzed");
}

fn print_game_state(Game game) -> void {
    log("=== Game State ===");
    log(game.title);
    show_num(game.level);
    
    log("Player 1:");
    log(game.player1.name);
    show_num(game.player1.health);
    show_num(game.player1.score);
    
    log("Player 2:");
    log(game.player2.name);
    show_num(game.player2.health);
    show_num(game.player2.score);
}

// Симуляция игрового цикла
fn simulate_game_round(Game game) -> Game {
    log("Starting game round simulation");
    
    Game updated_game = game;
    
    // Движение игроков
    Vector2D velocity1 = create_vector(1.0, 0.5);
    Vector2D velocity2 = create_vector(-0.5, 1.0);
    
    updated_game.player1 = move_player(updated_game.player1, velocity1, 0.1);
    updated_game.player2 = move_player(updated_game.player2, velocity2, 0.1);
    
    // Проверка коллизий
    bool collision = check_collision(updated_game.player1, updated_game.player2, 4.0);
    
    if (collision) {
        log("Collision detected!");
        updated_game.player1 = apply_damage(updated_game.player1, 10);
        updated_game.player2 = apply_damage(updated_game.player2, 10);
        
        updated_game.player1 = add_score(updated_game.player1, 5);
        updated_game.player2 = add_score(updated_game.player2, 5);
    }
    
    // Добавление очков за выживание
    updated_game.player1 = add_score(updated_game.player1, 1);
    updated_game.player2 = add_score(updated_game.player2, 1);
    
    return updated_game;
}

// Бенчмарк производительности
fn performance_test() -> void {
    log("=== Performance Test ===");
    
    auto iterations = 1000;
    
    for (i32 i = 0; i < iterations; i = i + 1) {
        auto test1 = lerp(0, 100, 0.5);
        auto test2 = clamp(i, 0, 500);
        auto test3 = distance(0, 0, i, i);
        auto test4 = max(test1, test2);
    }
    
    log("Performance test completed");
    show_num(iterations);
}

fn main() -> void {
    io::print("=== Echo Language Advanced Showcase ===");
    log("Demonstrating comprehensive language features");
    io::print("");
    
    // Создание игрового мира
    log("Creating game world...");
    
    Vector2D pos1 = create_vector(0.0, 0.0);
    Vector2D pos2 = create_vector(10.0, 10.0);
    
    Player player1 = create_player("Alice", 100, pos1);
    Player player2 = create_player("Bob", 100, pos2);
    
    Game game = Game {
        title: "Echo Arena",
        level: 1,
        player1: player1,
        player2: player2
    };
    
    log("Game world created");
    print_game_state(game);
    
    // Анализ начального состояния
    io::print("");
    analyze_player(game.player1);
    analyze_player(game.player2);
    
    // Симуляция игры
    io::print("");
    log("Running game simulation...");
    
    Game final_game = game;
    for (i32 round = 1; round <= 5; round = round + 1) {
        final_game = simulate_game_round(final_game);
        final_game.level = round;
    }
    
    // Финальное состояние
    io::print("");
    log("=== Final Game State ===");
    print_game_state(final_game);
    
    // Определение победителя
    io::print("");
    log("=== Game Results ===");
    if (final_game.player1.score > final_game.player2.score) {
        log("Winner: Player 1");
        log(final_game.player1.name);
    } else {
        if (final_game.player2.score > final_game.player1.score) {
            log("Winner: Player 2");
            log(final_game.player2.name);
        } else {
            log("Game ended in a tie!");
        }
    }
    
    // Бенчмарк
    io::print("");
    performance_test();
    
    // Итоговая статистика
    io::print("");
    io::print("=== Showcase Summary ===");
    log("✓ Complex nested structures");
    log("✓ Generic algorithms with multiple types");
    log("✓ Module system with import aliases");
    log("✓ Mathematical computations");
    log("✓ Game simulation with collision detection");
    log("✓ Conditional logic and state management");
    log("✓ Performance testing loops");
    log("✓ Comprehensive type system usage");
    
    io::print("");
    io::print("🎉 Echo Language Showcase Complete!");
    log("Production-ready capabilities demonstrated");
    log("Combining performance with expressiveness!");
} 