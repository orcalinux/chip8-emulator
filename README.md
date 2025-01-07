# CHIP-8 Emulator

![License](https://img.shields.io/badge/license-MIT-blue.svg) ![C](https://img.shields.io/badge/language-C-brightgreen.svg) ![SDL2](https://img.shields.io/badge/SDL2-yes-blue.svg)

A **CHIP-8 emulator** written in C, utilizing SDL2 for graphics and input. This project aims to faithfully replicate the behavior of the CHIP-8 virtual machine, providing a platform to run classic CHIP-8 programs and games.

---

## Table of Contents

- [CHIP-8 Emulator](#chip-8-emulator)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [Project Structure](#project-structure)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
      - [Installing Dependencies](#installing-dependencies)
    - [Building the Emulator](#building-the-emulator)
  - [Usage](#usage)
    - [Running the Emulator](#running-the-emulator)
      - [Example](#example)
    - [Command-Line Arguments](#command-line-arguments)
  - [Dependencies](#dependencies)
  - [Development](#development)
    - [Code Documentation](#code-documentation)
      - [Generating Documentation](#generating-documentation)
    - [Building in Debug Mode](#building-in-debug-mode)
  - [Contributing](#contributing)
    - [How to Contribute](#how-to-contribute)
    - [Coding Standards](#coding-standards)
  - [License](#license)
  - [Acknowledgements](#acknowledgements)

---

## Overview

The **CHIP-8 Emulator** is a software recreation of the CHIP-8 virtual machine, a simple interpreted programming language used in the 1970s to create games. This emulator allows you to run CHIP-8 programs on modern hardware, providing a nostalgic experience of classic gaming while serving as an educational tool for understanding emulation and low-level programming.

---

## Features

-   **Accurate Emulation**: Faithfully replicates CHIP-8 instruction set and behavior.
-   **SDL2 Integration**: Utilizes SDL2 for rendering graphics and handling input, ensuring smooth performance and compatibility across platforms.
-   **Modular Design**: Organized codebase with clear separation between emulator core and SDL interface.
-   **Configurable Display**: Customize window size via command-line arguments.
-   **Robust Makefile**: Simplifies building with support for debug and optimized builds.
-   **Doxygen Documentation**: Comprehensive code documentation for easier maintenance and understanding.

---

## Project Structure

```

chip8-emulator/
├── bin
├── include
│   ├── chip8.h
│   └── sdl_interface.h
├── Makefile
├── obj
│   ├── chip8.d
│   ├── main.d
│   └── sdl_interface.d
├── README.md
└── src
    ├── chip8.c
    ├── main.c
    └── sdl_interface.c
```

-   **`bin/`**: Contains the final compiled executable (`chip8`).
-   **`include/`**: Header files (`.h`) for the emulator core and SDL interface.
-   **`src/`**: Source files (`.c`) implementing the emulator logic and SDL interactions.
-   **`obj/`**: Compiled object files (`.o`) and dependency files (`.d`).
-   **`Makefile`**: Build script to compile the project.
-   **`README.md`**: Project documentation.

---

## Installation

### Prerequisites

Ensure you have the following installed on your system:

-   **GCC Compiler**: Supports C17 standard.
-   **SDL2 Development Libraries**: Required for graphics and input handling.

#### Installing Dependencies

-   **Ubuntu/Debian**:

    ```bash
    sudo apt-get update
    sudo apt-get install build-essential libsdl2-dev
    ```

-   **macOS** (using Homebrew):

    ```bash
    brew install sdl2
    ```

-   **Windows**:
    -   Install [MinGW](http://www.mingw.org/) for GCC.
    -   Download SDL2 development libraries from [SDL's official website](https://www.libsdl.org/download-2.0.php) and set them up according to your compiler.

### Building the Emulator

1. **Clone the Repository**:

    ```bash
    git clone https://github.com/yourusername/chip8-emulator.git
    cd chip8-emulator
    ```

2. **Build with Make**:

    ```bash
    make
    ```

    - **Debug Build**:
        ```bash
        make DEBUG=1
        ```

    The executable will be located in the `bin/` directory.

---

## Usage

### Running the Emulator

```bash
./bin/chip8 <path_to_CHIP8_ROM> [width] [height]
```

-   **`<path_to_CHIP8_ROM>`**: Path to the CHIP-8 program you wish to run.
-   **`[width]`**: (Optional) Width of the emulator window in pixels. Default is `640`.
-   **`[height]`**: (Optional) Height of the emulator window in pixels. Default is `320`.

#### Example

```bash
./bin/chip8 roms/PONG.ch8 800 600
```

This command runs the `PONG.ch8` ROM in an 800x600 window.

### Command-Line Arguments

-   **`<ROM>`**: Mandatory argument specifying the CHIP-8 program to execute.
-   **`<width> <height>`**: Optional arguments to customize the emulator window size.

---

## Dependencies

-   **C17 Compiler**: GCC or any compatible compiler supporting the C17 standard.
-   **SDL2**: Simple DirectMedia Layer 2.0 for handling graphics, input, and audio.

---

## Development

### Code Documentation

This project uses **Doxygen** for generating documentation from annotated source code.

#### Generating Documentation

1. **Install Doxygen**:

    - **Ubuntu/Debian**:
        ```bash
        sudo apt-get install doxygen
        ```
    - **macOS**:
        ```bash
        brew install doxygen
        ```

2. **Generate Documentation**:

    ```bash
    doxygen Doxyfile
    ```

    Ensure you have a `Doxyfile` configured for your project. You can generate a template with:

    ```bash
    doxygen -g
    ```

3. **View Documentation**:
   Open the generated `html/index.html` file in your web browser.

### Building in Debug Mode

To build the emulator with debug symbols and without optimization (useful for debugging):

```bash
make DEBUG=1
```

This adds the `-g` and `-O0` flags to the compiler.

---

## Contributing

Contributions are welcome! Whether you're fixing bugs, improving documentation, or adding new features, your input is valuable.

### How to Contribute

1. **Fork the Repository**:
   Click the "Fork" button on the repository page.

2. **Clone Your Fork**:

    ```bash
    git clone https://github.com/orcalinux/chip8-emulator.git
    cd chip8-emulator
    ```

3. **Create a Feature Branch**:

    ```bash
    git checkout -b feature/your-feature-name
    ```

4. **Commit Your Changes**:

    ```bash
    git commit -m "Add your descriptive commit message here"
    ```

5. **Push to Your Fork**:

    ```bash
    git push origin feature/your-feature-name
    ```

6. **Create a Pull Request**:
   Navigate to the original repository and create a pull request from your fork.

### Coding Standards

-   Follow the existing code style and conventions.
-   Ensure your code is well-documented with Doxygen comments.
-   Write clear and concise commit messages.

---

## License

This project is licensed under the [MIT License](LICENSE). You are free to use, modify, and distribute it as per the terms of the license.

---

## Acknowledgements

-   **CHIP-8**: Thanks to the original creators for designing a simple yet powerful virtual machine.
-   **SDL2 Community**: For providing a robust library for multimedia handling.
-   **Open-Source Contributors**: For their invaluable contributions to the tools and libraries that make projects like this possible.
