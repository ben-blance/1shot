CC=gcc
CFLAGS=-Wall -O2 -std=c99 -Iinclude
LIBS=-lgdi32 -luser32 -lshell32 -lcomctl32 -ladvapi32

# Source files in src directory
SRCDIR=src
BUILDDIR=build
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET=$(BUILDDIR)/1shot.exe

# Create build directory if it doesn't exist
$(BUILDDIR):
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"

# Default target
all: $(BUILDDIR) $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	@echo Linking $(TARGET)...
	@$(CC) $(OBJECTS) -o $(TARGET) $(LIBS) -mwindows

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo Cleaning build files...
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"

# Run the application
run: $(TARGET)
	@echo Running 1Shot...
	@$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Release build (default is already optimized)
release: all

# Show help
help:
	@echo Available targets:
	@echo   all      - Build the application (default)
	@echo   clean    - Clean build files
	@echo   run      - Build and run the application
	@echo   debug    - Build with debug symbols
	@echo   release  - Build optimized release version
	@echo   help     - Show this help message

.PHONY: all clean run debug release help