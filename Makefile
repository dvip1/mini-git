# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -Icomponents/include

# Directories
SRC_DIR = components
BUILD_DIR = build
COMMANDS_DIR = $(SRC_DIR)/commands
HELPER_DIR = $(SRC_DIR)/helper

# Source files
MAIN_SRC = main.c
COMMAND_SRCS = $(wildcard $(COMMANDS_DIR)/*.c)
HELPER_SRCS = $(wildcard $(HELPER_DIR)/*.c)
ALL_SRCS = $(MAIN_SRC) $(COMMAND_SRCS) $(HELPER_SRCS)

# Object files
MAIN_OBJ = $(BUILD_DIR)/main.o
COMMAND_OBJS = $(COMMAND_SRCS:$(COMMANDS_DIR)/%.c=$(BUILD_DIR)/%.o)
HELPER_OBJS = $(HELPER_SRCS:$(HELPER_DIR)/%.c=$(BUILD_DIR)/%.o)
ALL_OBJS = $(MAIN_OBJ) $(COMMAND_OBJS) $(HELPER_OBJS)

# Dependency files
DEPS = $(ALL_OBJS:.o=.d)

# Target executable
TARGET = git-using-c

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $@

# Compile main.c
$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile command source files
$(BUILD_DIR)/%.o: $(COMMANDS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile helper source files
$(BUILD_DIR)/%.o: $(HELPER_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Include dependency files
-include $(DEPS)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Install (optional - customize as needed)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run the program with arguments (use: make run-args ARGS="arg1 arg2 arg3")
run-args: $(TARGET)
	./$(TARGET) $(ARGS)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and build"
	@echo "  run      - Build and run the program"
	@echo "  run-args - Build and run with arguments (use: make run-args ARGS=\"arg1 arg2\")"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release version"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  help     - Show this help message"

# Declare phony targets
.PHONY: all clean rebuild install uninstall run run-args debug release help
