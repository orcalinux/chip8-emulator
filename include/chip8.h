/**
 * @file chip8.h
 * @brief CHIP-8 core emulator definitions
 *
 * This header declares the functions and data structures
 * for the core CHIP-8 emulation logic.
 */

#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/**
 * @brief CHIP-8 fontset for hexadecimal digits 0-F.
 *
 * Standard 4x5 fontset for digits 0-F, each character is 5 bytes (rows).
 * Typically loaded into memory starting at address 0x000.
 * Each byte represents one row of 8 pixels (only 4 or 5 are used).
 */
static const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * @enum chip8_state_t
 * @brief Represents the current state of the CHIP-8 emulator.
 */
typedef enum
{
    CHIP8_RUNNING, /**< Emulator is actively executing instructions */
    CHIP8_PAUSED,  /**< Emulator is paused (waiting for input or debugging) */
    CHIP8_STOPPED, /**< Emulator has stopped or terminated */
    CHIP8_ERROR    /**< Emulator encountered an error */
} chip8_state_t;

/**
 * @struct chip8_t
 * @brief Holds the state of the CHIP-8 emulator.
 */
typedef struct
{
    /* 8-byte aligned fields first */
    uint64_t last_timer_ticks; /**< Last time timers were updated (8 bytes) */
    chip8_state_t state;       /**< Current emulator state (4 bytes) */

    /* 16-bit arrays (stack) and registers grouped */
    uint16_t stack[16]; /**< 32 bytes */
    uint16_t I;         /**< Index register (2 bytes) */
    uint16_t pc;        /**< Program counter (2 bytes) */

    /* 8-bit fields */
    uint8_t sp;          /**< Stack pointer (1 byte) */
    uint8_t delay_timer; /**< Delay timer (1 byte) */
    uint8_t sound_timer; /**< Sound timer (1 byte) */
    uint8_t pad1;        /**< Padding to align to 8 bytes */

    /* 8-bit arrays */
    uint8_t V[16]; /**< CPU registers V0..VF (16 bytes) */
    bool keys[16]; /**< Input keys 0x0..0xF (16 bytes if bool is 1 byte) */

    /* Large bool array (display) */
    bool display[64 * 32]; /**< 2048 bytes if bool is 1 byte */

    /* Largest array last: 4K memory */
    uint8_t memory[4096]; /**< 4096 bytes */

    /* the display config structure (280 bytes) */
    display_config_t config; /**< Aligned to 8 bytes to avoid extra padding */
} chip8_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the CHIP-8 emulator state.
     *
     * @param emu Pointer to the chip8_t struct to be initialized.
     * @return true on success, false on error.
     */
    bool chip8_init(chip8_t *emu);

    /**
     * @brief Loads a CHIP-8 program into memory.
     *
     * @param emu Pointer to the chip8_t struct.
     * @param filepath Path to the CHIP-8 ROM file.
     * @return true on success, false on error.
     */
    bool chip8_load_program(chip8_t *emu, const char *filepath);

    /**
     * @brief Executes one cycle (one or more opcodes) of the CHIP-8 CPU.
     *
     * @param emu Pointer to the chip8_t struct.
     */
    void chip8_cycle(chip8_t *emu);

    /**
     * @brief Decrements CHIP-8 emulator timers.
     *
     * Decrements both the delay and sound timers if they are non-zero.
     * This operation typically occurs at a fixed rate of 60 Hz to follow
     * the CHIP-8 specification.
     *
     * @param emu Pointer to the CHIP-8 emulator context.
     */
    void chip8_timers_decrement(chip8_t *emu);

    /**
     * @brief Ticks the emulator's delay and sound timers at a 60 Hz rate.
     *
     * This routine updates the internal timers to emulate the original CHIP-8
     * behavior, where both the delay and sound timers count down at a rate of
     * 60 times per second.
     *
     * @param emu Pointer to the CHIP-8 emulator instance.
     */
    void chip8_timers_tick_60hz(chip8_t *emu);

#ifdef __cplusplus
}
#endif

#endif /* CHIP8_H */
