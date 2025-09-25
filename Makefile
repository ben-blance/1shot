CC=gcc
CFLAGS=-Wall -Iinclude
LIBS=-lgdi32 -luser32 -lshell32

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
OUT=build/1shot.exe

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS) -mwindows

clean:
	del /Q src\*.o build\*.exe
