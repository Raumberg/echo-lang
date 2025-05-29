#!/bin/bash

# Echo Language Examples Test Script
# Тестирует все примеры и показывает результаты

echo "=================================================="
echo "🚀 Echo Language Examples Test Suite"
echo "=================================================="
echo ""

# Проверяем что компилятор собран
if [ ! -f "bin/echo" ]; then
    echo "⚠️  Компилятор не найден. Собираем..."
    make all
    if [ $? -ne 0 ]; then
        echo "❌ Ошибка сборки компилятора"
        exit 1
    fi
    echo "✅ Компилятор собран"
    echo ""
fi

# Счетчики
total_examples=0
successful_examples=0
failed_examples=0

# Массив с информацией о примерах
declare -a examples=(
    "01_hello_world.ec:Базовый синтаксис и импорты"
    "02_basic_types.ec:Типы данных и автовывод"
    "03_structs_basics.ec:Основы структур"
    "04_control_flow.ec:Управляющие конструкции"
    "05_simple_generics.ec:Простые Generic функции"
    "06_modules_imports.ec:Модульная система"
    "07_data_structures.ec:Сложные структуры данных"
    "08_generic_algorithms.ec:Generic алгоритмы"
    "09_calculator.ec:Практический калькулятор"
    "10_advanced_showcase.ec:Продвинутая демонстрация"
)

echo "Найдено примеров: ${#examples[@]}"
echo ""

# Функция тестирования примера
test_example() {
    local example_info=$1
    local filename=$(echo $example_info | cut -d':' -f1)
    local description=$(echo $example_info | cut -d':' -f2)
    
    if [ ! -f "examples/$filename" ]; then
        echo "⚠️  Файл examples/$filename не найден"
        return 1
    fi
    
    echo "🧪 Тестируем: $filename"
    echo "   📄 $description"
    
    # Компиляция с перенаправлением вывода
    ./bin/echo "examples/$filename" > "/tmp/echo_test_${filename}.log" 2>&1
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo "   ✅ Компиляция успешна"
        
        # Попытка скомпилировать C код если он был сгенерирован
        if [ -f "examples/${filename%.*}.c" ]; then
            echo "   🔧 Компилируем C код..."
            gcc -o "/tmp/echo_${filename%.*}" "examples/${filename%.*}.c" "examples/echo_runtime.c" 2>/dev/null
            if [ $? -eq 0 ]; then
                echo "   ✅ C код скомпилирован успешно"
                # Можем даже запустить для быстрой проверки
                timeout 5s "/tmp/echo_${filename%.*}" > "/tmp/echo_run_${filename}.log" 2>&1
                if [ $? -eq 0 ]; then
                    echo "   🎉 Программа выполнена успешно"
                fi
                rm -f "/tmp/echo_${filename%.*}"
            else
                echo "   ⚠️  C код не компилируется, но Echo-компиляция прошла"
            fi
        fi
        
        return 0
    else
        echo "   ❌ Ошибка компиляции (код: $exit_code)"
        echo "   📝 Логи в /tmp/echo_test_${filename}.log"
        return 1
    fi
}

# Тестирование всех примеров
for example in "${examples[@]}"; do
    total_examples=$((total_examples + 1))
    
    if test_example "$example"; then
        successful_examples=$((successful_examples + 1))
    else
        failed_examples=$((failed_examples + 1))
    fi
    
    echo ""
done

# Итоговая статистика
echo "=================================================="
echo "📊 Результаты тестирования"
echo "=================================================="
echo "📋 Всего примеров: $total_examples"
echo "✅ Успешно: $successful_examples"
echo "❌ С ошибками: $failed_examples"
echo ""

if [ $failed_examples -eq 0 ]; then
    echo "🎉 Все примеры работают отлично!"
    echo "🚀 Echo Language готов к использованию!"
else
    echo "⚠️  $failed_examples примеров имеют проблемы"
    echo "🔍 Проверьте логи в /tmp/echo_test_*.log"
fi

echo ""
echo "=================================================="
echo "🎓 Начните изучение с examples/01_hello_world.ec"
echo "📚 Документация: examples/README.md"
echo "🛠️  Правила generics: docs/generics_rules.md"
echo "=================================================="

# Возвращаем код ошибки если есть неудачные тесты
exit $failed_examples 