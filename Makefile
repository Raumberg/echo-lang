CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O0
SRCDIR = src
TESTDIR = tests
OBJDIR = build
BINDIR = bin

# Source files (recursively find all .c files)
SOURCES = $(shell find $(SRCDIR) -name "*.c")
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Test files
TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(OBJDIR)/%.o)

# Main targets
COMPILER = $(BINDIR)/echo
TEST_RUNNER = $(BINDIR)/test_runner

.PHONY: all clean test directories

all: directories $(COMPILER)

# Create necessary directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)
	@mkdir -p $(OBJDIR)/lexer $(OBJDIR)/parser $(OBJDIR)/ast $(OBJDIR)/semantic $(OBJDIR)/codegen $(OBJDIR)/utils $(OBJDIR)/runtime

# Build main compiler
$(COMPILER): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Build object files (handle subdirectories)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c $< -o $@

# Build test runner
test: directories $(TEST_RUNNER)
	./$(TEST_RUNNER)

$(TEST_RUNNER): $(TEST_OBJECTS) $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -I$(SRCDIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Development helpers
dev: all
	@echo "Echo compiler built successfully!"
	@echo "Run './$(COMPILER) <file.ec>' to compile Echo code"

# Test with example files
test-examples: all
	@echo "Testing parser with example files..."
	@if [ -f examples/hello.ec ]; then \
		echo "=== Testing hello.ec ==="; \
		./$(COMPILER) examples/hello.ec; \
	fi
	@if [ -f examples/pointers.ec ]; then \
		echo "=== Testing pointers.ec ==="; \
		./$(COMPILER) examples/pointers.ec; \
	fi
	@if [ -f examples/memory.ec ]; then \
		echo "=== Testing memory.ec ==="; \
		./$(COMPILER) examples/memory.ec; \
	fi

# Separate test targets
test-lexer: directories $(TEST_RUNNER)
	./$(TEST_RUNNER)

# Test parser specifically
test-parser: all
	@echo "Testing parser with simple examples..."
	@echo 'fn main() -> i32 { return 42; }' > /tmp/test.ec
	./$(COMPILER) /tmp/test.ec
	@rm -f /tmp/test.ec

# Test parser unit tests
test-parser-unit: directories
	$(CC) $(CFLAGS) -I$(SRCDIR) $(TESTDIR)/test_parser.c $(filter-out $(OBJDIR)/main.o, $(OBJECTS)) -o $(BINDIR)/test_parser
	./$(BINDIR)/test_parser

# Test semantic analysis unit tests
test-semantic-unit: directories
	$(CC) $(CFLAGS) -I$(SRCDIR) $(TESTDIR)/test_semantic.c $(filter-out $(OBJDIR)/main.o, $(OBJECTS)) -o $(BINDIR)/test_semantic
	./$(BINDIR)/test_semantic

# Test semantic analysis with examples
test-semantic: all
	@echo "Testing semantic analysis with examples..."
	@echo 'fn main() -> i32 { return 42; }' > /tmp/test_semantic.ec
	./$(COMPILER) /tmp/test_semantic.ec
	@rm -f /tmp/test_semantic.ec

# Run all tests
test-all: test-parser-unit test-semantic-unit test-examples
	@echo "All tests completed!"

install:
	cp $(COMPILER) /usr/local/bin/

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all               - Build the Echo compiler"
	@echo "  test              - Run all tests"
	@echo "  test-lexer        - Run lexer tests only"
	@echo "  test-parser       - Test parser with simple examples"
	@echo "  test-parser-unit  - Run parser unit tests"
	@echo "  test-semantic     - Test semantic analysis with examples"
	@echo "  test-semantic-unit- Run semantic analysis unit tests"
	@echo "  test-examples     - Test with .ec example files"
	@echo "  test-all          - Run all unit tests and examples"
	@echo "  clean             - Remove build files"
	@echo "  dev               - Build and show usage info"
	@echo "  install           - Install compiler to /usr/local/bin" 