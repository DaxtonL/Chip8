CC = clang

CFLAGS = -Wall -Wextra -std=c11 $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs-only-L sdl2)
LDLIBS = $(shell pkg-config --libs-only-l sdl2)

TARGET = build/chip8

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))

ROM_DIR = roms

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) roms/$(filter-out run,$(MAKECMDGOALS)) 2>/dev/null

clean:
	rm -rf build/*

%:
	@: