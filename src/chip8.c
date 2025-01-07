/**
 * @file chip8.c
 * @brief Implementation of the CHIP-8 core emulator.
 */

#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

bool chip8_init(chip8_t *emu)
{
    // Clear the entire emulator struct
    memset(emu, 0, sizeof(chip8_t));

    // Set default emulator state
    emu->state = CHIP8_RUNNING;

    // Program counter starts at 0x200 (standard for most CHIP-8 programs)
    emu->pc = 0x200;

    // Load the fontset into memory starting at 0x000
    //    Typically 80 bytes for digits 0-F
    memcpy(emu->memory, chip8_fontset, sizeof(chip8_fontset));

    // Stack pointer, timers, and other fields default to 0 from memset
    emu->sp = 0;
    emu->delay_timer = 0;
    emu->sound_timer = 0;

    return true;
}

bool chip8_load_program(chip8_t *emu, const char *filepath)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp)
    {
        fprintf(stderr, "Failed to open ROM file: %s\n", filepath);
        return false;
    }

    // Read file into memory starting at 0x200
    size_t bytes_read = fread(&emu->memory[0x200], 1, 4096 - 0x200, fp);
    fclose(fp);

    if (bytes_read <= 0)
    {
        fprintf(stderr, "No data read from ROM: %s\n", filepath);
        return false;
    }

    return true;
}

void chip8_cycle(chip8_t *emu)
{
    // Fetch opcode
    // uint16_t opcode = (emu->memory[emu->pc] << 8) | emu->memory[emu->pc + 1];

    // Decode and execute opcode
    // For example, handle each nibble:
    // ...
    // increment program counter
    emu->pc += 2;
}

void chip8_timers_decrement(chip8_t *emu)
{
    if (emu->delay_timer > 0)
    {
        emu->delay_timer--;
    }

    if (emu->sound_timer > 0)
    {
        emu->sound_timer--;
        // Possibly beep if sound_timer hits 0
    }
}

void chip8_timers_tick_60hz(chip8_t *emu)
{
    static uint64_t lastCount = 0;
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t freq = SDL_GetPerformanceFrequency();

    // Initialize lastCount the first time
    if (lastCount == 0)
        lastCount = now;

    double elapsed = (double)(now - lastCount) / (double)freq;
    double interval = 1.0 / 60.0; // ~16.6667 ms

    // Decrement timers multiple times if behind
    while (elapsed >= interval)
    {
        chip8_timers_decrement(emu);
        elapsed -= interval;
        lastCount += (uint64_t)(interval * freq);
    }
}
