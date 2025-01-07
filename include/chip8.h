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
    uint8_t memory[4096];  /**< 4K memory for the CHIP-8 system */
    uint8_t V[16];         /**< CPU registers V0..VF */
    uint16_t I;            /**< Index register */
    uint16_t pc;           /**< Program counter */
    uint16_t stack[16];    /**< Call stack */
    uint8_t sp;            /**< Stack pointer */
    uint8_t delay_timer;   /**< Delay timer */
    uint8_t sound_timer;   /**< Sound timer */
    bool display[64 * 32]; /**< Monochrome display (64x32) */
    bool keys[16];         /**< Input keys 0x0..0xF */
    config_t config;       /**< Configuration settings */
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
     * @brief Decrements timers (delay and sound), typically called at 60 Hz.
     *
     * @param emu Pointer to the chip8_t struct.
     */
    void chip8_update_timers(chip8_t *emu);

#ifdef __cplusplus
}
#endif

#endif /* CHIP8_H */
