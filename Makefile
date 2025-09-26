CC=gcc
CFLAGS=-Wall -O2 -std=c99 -Iinclude
LIBS=-lgdi32 -luser32 -lshell32 -lcomctl32

SRCDIR=src
BUILDDIR=build
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET=$(BUILDDIR)/1shot.exe

$(BUILDDIR):
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"


all: $(BUILDDIR) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo Linking $(TARGET)...
	@$(CC) $(OBJECTS) -o $(TARGET) $(LIBS) -mwindows

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -c $< -o $@


clean:
	@echo Cleaning build files...
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"

run: $(TARGET)
	@echo Running 1Shot...
	@$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean all

release: all

help:
	@echo Available targets:
	@echo   all      - Build the application (default)
	@echo   clean    - Clean build files
	@echo   run      - Build and run the application
	@echo   debug    - Build with debug symbols
	@echo   release  - Build optimized release version
	@echo   help     - Show this help message

.PHONY: all clean run debug release help