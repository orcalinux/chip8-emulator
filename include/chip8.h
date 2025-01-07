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
 * @struct chip8_t
 * @brief Holds the state of the CHIP-8 emulator.
 */
typedef struct
{
    /*
     * 1) 8-byte (64-bit) fields first:
     *    - Ensures no wasted alignment gap at the start.
     */
    uint64_t last_timer_ticks; /**< Last time the timers were updated (8 bytes) */

    /*
     * 2) Structures that align on 4 or 8 bytes (like config_t, which has only uint32_t fields).
     *    - Starting at offset 8 should keep it properly aligned on most platforms.
     */
    config_t config; /**< Configuration (4x uint32_t) ~ 16 bytes */

    /*
     * 3) Larger 16-bit arrays:
     *    - Grouping 16-bit items together can reduce repeated alignment overhead.
     *    - stack[16] is 32 bytes total (16 * 2).
     */
    uint16_t stack[16]; /**< Call stack (32 bytes) */

    /*
     * 16-bit registers:
     */
    uint16_t I;  /**< Index register (2 bytes) */
    uint16_t pc; /**< Program counter (2 bytes) */

    /*
     * 4) 8-bit fields:
     *    - Group them so only minimal or no padding is needed.
     */
    uint8_t sp;          /**< Stack pointer (1 byte) */
    uint8_t delay_timer; /**< Delay timer (1 byte) */
    uint8_t sound_timer; /**< Sound timer (1 byte) */
    uint8_t pad1;        /**< 1 byte of padding to align next items, if needed */

    /*
     * 5) 8-bit arrays:
     *    - V[16] is 16 bytes, keys[16] is 16 bytes (if bool is 1 byte).
     */
    uint8_t V[16]; /**< CPU registers V0..VF (16 bytes) */
    bool keys[16]; /**< Input keys 0x0..0xF (16 bytes, if bool == 1 byte) */

    /*
     * 6) Large bool array:
     *    - 64*32 = 2048, typically 1 byte each for bool on most compilers.
     *      Placing it next means we won't introduce alignment gaps inside it.
     */
    bool display[64 * 32]; /**< Monochrome display (2048 bytes if bool is 1 byte) */

    /*
     * 7) Biggest array last:
     *    - 4096 bytes for memory is simply appended.
     *      No further alignment concerns once we're at the end.
     */
    uint8_t memory[4096]; /**< 4K memory for the CHIP-8 system */

} chip8_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the CHIP-8 emulator state.
     *
     * @param emu Pointer to the chip8_t struct to be initialized.
     */
    void chip8_init(chip8_t *emu);

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
