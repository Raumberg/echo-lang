// Generated by Echo Language Compiler
// Do not edit this file manually

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "echo_runtime.h"

// Type definitions

// Function declarations
void main(void);
double add_integer_float(int a, double b);
int factorial_integer(int n);
int power_integer_integer(int base, int exp);
int percentage_integer_integer(int value, int percent);
int32_t abs_value_i32(int32_t x);
int square_integer(int x);
bool is_zero_integer(int x);
double multiply_float_float(double a, double b);
double add_float_float(double a, double b);
int divide_integer_integer(int a, int b);
int multiply_integer_integer(int a, int b);
int subtract_integer_integer(int a, int b);
int add_integer_integer(int a, int b);

void main(void) {
    echo_print_string("=== Simple Calculator Demo ===");
    echo_print_string("Basic arithmetic:");
    int sum = add_integer_integer(15, 25);
    int diff = subtract_integer_integer(50, 20);
    int product = multiply_integer_integer(7, 8);
    int quotient = divide_integer_integer(100, 4);
    echo_print_int(sum);
    echo_print_int(diff);
    echo_print_int(product);
    echo_print_int(quotient);
    echo_print_string("");
    echo_print_string("Float calculations:");
    double float_sum = add_float_float(3.14, 2.86);
    double float_product = multiply_float_float(2.5, 4.0);
    echo_print_string("Float operations completed");
    echo_print_string("");
    echo_print_string("Division by zero test:");
    int zero_result = divide_integer_integer(10, 0);
    if (is_zero_integer(zero_result)) {
        echo_print_string("Division by zero handled correctly");
    }
    echo_print_string("");
    echo_print_string("Advanced functions:");
    int squared = square_integer(8);
    int32_t abs_val = abs_value_i32(-15);
    int percent_val = percentage_integer_integer(200, 15);
    echo_print_int(squared);
    echo_print_int(abs_val);
    echo_print_int(percent_val);
    echo_print_string("");
    echo_print_string("Power calculations:");
    int power_2_3 = power_integer_integer(2, 3);
    int power_5_2 = power_integer_integer(5, 2);
    echo_print_int(power_2_3);
    echo_print_int(power_5_2);
    echo_print_string("");
    echo_print_string("Factorial calculations:");
    int fact_4 = factorial_integer(4);
    int fact_5 = factorial_integer(5);
    echo_print_int(fact_4);
    echo_print_int(fact_5);
    echo_print_string("");
    echo_print_string("Complex calculations:");
    int complex1 = add_integer_float(square_integer(5), multiply_integer_integer(3, 4));
    int complex2 = subtract_integer_integer(power_integer_integer(3, 3), factorial_integer(3));
    echo_print_int(complex1);
    echo_print_int(complex2);
    echo_print_string("");
    echo_print_string("Generic type flexibility:");
    int int_add = add_integer_integer(10, 20);
    int float_add = add_float_float(1.5, 2.5);
    double mixed_calc = multiply_integer_integer(divide_integer_integer(100, 4), 2);
    echo_print_int(int_add);
    echo_print_string("Float addition completed");
    echo_print_int(mixed_calc);
    echo_print_string("");
    echo_print_string("=== Calculator Features ===");
    echo_print_string("✓ Basic arithmetic (+, -, *, /)");
    echo_print_string("✓ Error handling (division by zero)");
    echo_print_string("✓ Advanced functions (square, abs, %)");
    echo_print_string("✓ Power and factorial calculations");
    echo_print_string("✓ Generic functions for type flexibility");
    echo_print_string("✓ Complex expression evaluation");
    echo_print_string("");
    echo_print_string("Calculator demo completed successfully!");
}

double add_integer_float(int a, double b) {
    return a + b;
}

int factorial_integer(int n) {
    if (n <= 1) {
        return 1;
    }
    else {
        int32_t prev = factorial_integer(n - 1);
        return n * prev;
    }
}

int power_integer_integer(int base, int exp) {
    if (exp == 0) {
        return 1;
    }
    int result = base;
    for (int32_t i = 1; i < exp; i = i + 1) {
        result = result * base;
    }
    return result;
}

int percentage_integer_integer(int value, int percent) {
    return value * percent / 100;
}

int32_t abs_value_i32(int32_t x) {
    if (x < 0) {
        return -x;
    }
    else {
        return x;
    }
}

int square_integer(int x) {
    return x * x;
}

bool is_zero_integer(int x) {
    return x == 0;
}

double multiply_float_float(double a, double b) {
    return a * b;
}

double add_float_float(double a, double b) {
    return a + b;
}

int divide_integer_integer(int a, int b) {
    if (b != 0) {
        return a / b;
    }
    else {
        return 0;
    }
}

int multiply_integer_integer(int a, int b) {
    return a * b;
}

int subtract_integer_integer(int a, int b) {
    return a - b;
}

int add_integer_integer(int a, int b) {
    return a + b;
}

