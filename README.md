# CHIP-8 Emulator

A CHIP-8 emulator written in C using SDL2 for rendering and keyboard input.

## Requirements

* macOS / Linux
* Clang or GCC
* Make
* SDL2
* `pkg-config`

### macOS

Install the dependencies using Homebrew:

```bash
brew install sdl2 pkg-config
```

---

# Using the Makefile

## Build the Emulator

To compile the emulator:

```bash
make
```

This will:

1. Compile each `.c` file in `src/`
2. Create the corresponding `.o` files in `build/`
3. Link the object files together
4. Create the executable:

```text
build/chip8
```

You can also explicitly use:

```bash
make all
```

---

## Run a ROM

The easiest way to run a ROM is:

```bash
make run <rom>
```

For example:

```bash
make run ibm.ch8
```

This is equivalent to:

```bash
./build/chip8 roms/ibm.ch8
```

The ROM must be located inside the `roms/` directory.

### Example

```text
roms/
├── ibm.ch8
├── pong.ch8
└── test.ch8
```

You can run:

```bash
make run pong.ch8
```

---

# ROMs

CHIP-8 ROMs should be placed in:

```text
roms/
```

For example:

```text
roms/pong.ch8
```

ROM files are excluded from version control through `.gitignore`.

---

# Keyboard Layout

The CHIP-8 keypad is mapped to the keyboard as follows:

```text
CHIP-8       Keyboard

1 2 3 C      1 2 3 4
4 5 6 D      Q W E R
7 8 9 E      A S D F
A 0 B F      Z X C V
```

---


# License

This project is for educational purposes.
