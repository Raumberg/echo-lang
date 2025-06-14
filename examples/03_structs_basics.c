// Generated by Echo Language Compiler
// Do not edit this file manually

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "echo_runtime.h"

// Type definitions

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    char* name;
    int32_t age;
    bool is_employed;
    Point location;
} Person;

// Function declarations
void main(void);

void main(void) {
    echo_print_string("=== Structs Basics Demo ===");
    Point origin = {.x = 0.0, .y = 0.0};
    Point destination = {.x = 10.5, .y = 20.7};
    echo_print_string("Points created:");
    echo_print_string("Origin and destination initialized");
    Person person = {.name = "Bob", .age = 30, .is_employed = true, .location = {.x = 100.0, .y = 200.0}};
    echo_print_string("");
    echo_print_string("Person info:");
    echo_print_string(person.name);
    echo_print_int(person.age);
    echo_print_bool(person.is_employed);
    echo_print_string("");
    echo_print_string("Person location:");
    echo_print_string("X and Y coordinates accessed");
    person.age = 31;
    person.is_employed = false;
    echo_print_string("");
    echo_print_string("After updates:");
    echo_print_int(person.age);
    echo_print_bool(person.is_employed);
}

