# CHIP-8 Emulator ROMs

Welcome to the **CHIP-8 Emulator ROMs** directory! This folder houses a curated collection of ROM files designed to work seamlessly with your CHIP-8 emulator. The ROMs are split into **Test ROMs** (for debugging and verifying emulator functionality) and **Game ROMs** (for experiencing classic CHIP-8 games).

---

## Table of Contents

- [CHIP-8 Emulator ROMs](#chip-8-emulator-roms)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [ROM Categories](#rom-categories)
    - [Test ROMs](#test-roms)
    - [Game ROMs](#game-roms)
  - [ROM Descriptions](#rom-descriptions)
  - [Adding New ROMs](#adding-new-roms)
  - [Running ROMs](#running-roms)
  - [Testing Your Emulator](#testing-your-emulator)
    - [Using `test_opcode.ch8`](#using-test_opcodech8)
    - [Additional Test ROMs](#additional-test-roms)
  - [Contributing](#contributing)
    - [Guidelines for Contributions](#guidelines-for-contributions)
  - [Licensing](#licensing)
    - [ROMs](#roms)
    - [Emulator](#emulator)
  - [Resources](#resources)

---

## Introduction

The ROMs in this directory serve two key purposes:

1. **Validation** – Ensuring that your CHIP-8 emulator implements all required opcodes and behaviors correctly.
2. **Fun and Demonstration** – Showcasing retro-style CHIP-8 games that let users experience the system’s capabilities.

By combining **Test ROMs** and **Game ROMs**, you can both **debug** emulator inaccuracies and **enjoy** classic, lightweight games originally written for this platform.

---

## ROM Categories

### Test ROMs

**Purpose**:  
These ROMs stress-test your emulator by covering a wide range of CHIP-8 instructions (opcodes) and edge cases.

-   **`test_opcode.ch8`**
    -   **Description**: A thorough test ROM exercising each opcode to confirm proper instruction handling and memory operations. This is pivotal for verifying your emulator’s correctness.

### Game ROMs

**Purpose**:  
Enjoy classic and modern homebrew games, while demonstrating the emulator’s handling of rendering, timing, input, and game logic.

-   **`pong.ch8`**
    -   **Description**: A two-player paddle game. Tests basic collision detection, user input, and sprite rendering.
-   **`tetris.ch8`**
    -   **Description**: The timeless block-stacking game, testing more complex sprite movements, rotations, and line-clearing mechanics.
-   **`space_invaders.ch8`**
    -   **Description**: A retro alien shooter port. Stresses multiple moving objects, collision checks, and sprite drawing.
-   **`breakout.ch8`**
    -   **Description**: A brick-breaking arcade classic. Exercises collision between ball, paddle, and bricks.
-   **`invaders.ch8`**
    -   **Description**: Another variant of space invaders, potentially with different mechanics or difficulty.
-   **`snake.ch8`**
    -   **Description**: Tests “snake” style movement, including dynamic growth of a sprite and boundary collision checks.

---

## ROM Descriptions

| **ROM Name**         | **Category** | **Purpose**                              | **Description**                                                          |
| -------------------- | ------------ | ---------------------------------------- | ------------------------------------------------------------------------ |
| `test_opcode.ch8`    | Test         | Validate Full Opcode Implementation      | Exercises **all** CHIP-8 opcodes to ensure correct instruction handling. |
| `pong.ch8`           | Game         | Basic Rendering & Collision              | A two-player paddle game for testing collision & user input.             |
| `tetris.ch8`         | Game         | Complex Sprite Movement & Rotation       | Handles line-clearing, falling blocks, and user input.                   |
| `space_invaders.ch8` | Game         | Multiple Objects & Collision Detection   | Emulates an alien shooter with collision checks & sprite rendering.      |
| `breakout.ch8`       | Game         | Physics and Collision Mechanics          | Brick-breaking game with ball, paddle, and brick collisions.             |
| `invaders.ch8`       | Game         | Enhanced Alien Shooter Variant           | Tests multiple enemy types or different difficulty mechanics.            |
| `snake.ch8`          | Game         | Continuous Movement & Boundary Collision | Snake-style gameplay, testing collision with self and walls.             |
| ...                  | ...          | ...                                      | ...                                                                      |

> _Note_: Add more entries as your collection grows.

---

## Adding New ROMs

1. **Obtain the ROM**

    - Download from sources like [dmatlack/chip8](https://github.com/dmatlack/chip8) or [Awesome CHIP-8 Resources](https://github.com/tobiasvl/awesome-chip-8).

2. **Place It in `roms/`**

    ```bash
    cp path/to/rom_name.ch8 roms/
    ```

3. **Update the README**

    - Add a row to [ROM Descriptions](#rom-descriptions) detailing name, category, purpose, and description.

4. **Test It**
    ```bash
    ./chip8-emulator roms/rom_name.ch8
    ```

---

## Running ROMs

1. **Navigate to Emulator Directory**

    ```bash
    cd ~/chip8-emulator
    ```

2. **Execute the Emulator**
    ```bash
    ./chip8-emulator [options] roms/rom_name.ch8
    ```
3. **Command-Line Options**
    - **`-w, --width <value>`**: Window width (e.g., `-w 640`).
    - **`-h, --height <value>`**: Window height (e.g., `-h 320`).
    - **`-s, --scale <value>`**: Scale factor for upscaling the 64x32 display (e.g., `-s 10`).
    - **`-f, --foreground <color>`**: Foreground color in hex (e.g., `-f FFFFFF`).
    - **`-b, --background <color>`**: Background color in hex (e.g., `-b 000000`).
    - **`--help`**: Show usage information.

**Example**:

```bash
./chip8-emulator -w 640 -h 320 -s 10 -f FFFFFF -b 000000 roms/pong.ch8
```

This sets the window size to 640×320, scale to 10, foreground (white) and background (black).

4. **Interactive Controls**
    - Press **Enter**: Displays text “Mahmoud love Sarah.”
    - Press **Esc**: Pauses or resumes the emulator (if implemented).
    - Press **Q**: Quits.

> _Ensure your emulator’s input mapping matches these keys or adjust accordingly._

---

## Testing Your Emulator

### Using `test_opcode.ch8`

1. **Run the Test ROM**
    ```bash
    ./chip8-emulator roms/test_opcode.ch8
    ```
2. **Observe Output**
    - The ROM executes a series of opcode tests, displaying relevant visuals or data.
3. **Debug**
    - If anomalies appear, investigate your emulator’s opcode handling and memory management.
    - Insert logging or breakpoints in `chip8_cycle` or related functions.

### Additional Test ROMs

-   Add specialized test ROMs focusing on timers, sound, or quirks (like SHIFT instructions).
-   Re-run these ROMs regularly to confirm no regressions after changes.

---

## Contributing

1. **Fork & Clone**
    ```bash
    git clone https://github.com/yourusername/chip8-emulator.git
    cd chip8-emulator
    ```
2. **Add ROM & README Entry**
    ```bash
    cp path/to/new_rom.ch8 roms/
    # Update roms/README.md with the new entry
    ```
3. **Commit & Push**
    ```bash
    git add roms/ README.md
    git commit -m "Add new_rom.ch8 and doc entry"
    git push origin main
    ```
4. **Open Pull Request**
    - Provide a concise description of your new ROM or documentation changes.

### Guidelines for Contributions

-   **Quality**: Include stable and recognized ROMs to ensure reliability.
-   **Clear Documentation**: Provide a brief but detailed reason for the ROM’s existence.
-   **Legal Compliance**: Only distribute ROMs you have the legal right to share.

---

## Licensing

### ROMs

-   **Test ROMs**: Usually open-licensed or created specifically for debugging.
-   **Game ROMs**: Must be in the public domain or otherwise legally distributable.
-   **No Unauthorized Distribution**: Avoid committing commercial or proprietary ROMs.

### Emulator

If your emulator is open-source, place the license (e.g., MIT, GPL) in a `LICENSE` file at the project root:

```markdown
# Licensing

This project is licensed under the MIT License - see [LICENSE](../LICENSE) for details.
```

---

## Resources

-   **CHIP-8 Technical References**
    -   [Wikipedia: CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)
    -   [Cowgod’s Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
-   **Emulator Development**
    -   [Write a CHIP-8 Emulator (tobiasvl)](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
-   **ROM Sources**

    -   [dmatlack/chip8](https://github.com/dmatlack/chip8)
    -   [Awesome CHIP-8 Resources](https://github.com/tobiasvl/awesome-chip-8)

-   **Hex & Conversion Tools**
    -   [xxd](https://linux.die.net/man/1/xxd) – Hex dump & convert
    -   [xclip](https://linux.die.net/man/1/xclip) – Clipboard operations
    -   [hexdump](https://linux.die.net/man/1/hexdump) – Hex representation
