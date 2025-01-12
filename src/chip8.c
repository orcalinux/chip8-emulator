/**
 * @file chip8.c
 * @brief Implementation of the CHIP-8 core emulator.
 */

#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_logger.h"
#include "chip8.h"
#include "audio.h"

bool chip8_init(chip8_t *emu)
{
    // Clear the entire emulator struct
    memset(emu, 0, sizeof(chip8_t));

    // Set default emulator state
    emu->state = CHIP8_RUNNING;

    // Program counter starts at 0x200 (standard for most CHIP-8 programs)
    emu->pc = CHIP8_ROM_ENTRY_POINT;

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
        print_error("Failed to open ROM file: %s\n", filepath);
        return false;
    }

    // Determine the file size
    fseek(fp, 0, SEEK_END);     // Move to the end of the file
    long file_size = ftell(fp); // Get the current file position (file size in bytes)
    rewind(fp);                 // Reset file pointer to the beginning

    // Validate the file size against available memory space
    const size_t max_rom_size = CHIP8_MEMORY_SIZE - CHIP8_ROM_ENTRY_POINT; // 4096 - 512 = 3584

    if (file_size < 0) // Check for error in ftell
    {
        print_error("Error determining ROM file size.\n");
        fclose(fp);
        return false;
    }

    if ((size_t)file_size > max_rom_size)
    {
        print_warning("ROM file too large: %ld bytes (max allowed: %zu bytes)\n", file_size, max_rom_size);
        fclose(fp);
        return false;
    }

    // Read the file into memory starting at the entry point (0x200)
    size_t bytes_read = fread(&emu->memory[CHIP8_ROM_ENTRY_POINT], 1, (size_t)file_size, fp);
    fclose(fp);

    if (bytes_read != (size_t)file_size)
    {
        print_error("Failed to read the entire ROM file: %s\n", filepath);
        return false;
    }

    print_info("Loaded ROM: %s (%ld bytes)\n", filepath, file_size);
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
    // Decrement the delay timer
    if (emu->delay_timer > 0)
        emu->delay_timer--;

    // Handle the sound timer
    if (emu->sound_timer > 0)
    {
        // If we're not already playing the beep, start looping it now
        if (!audio_is_beep_playing())
        {
            audio_play_beep_loop();
        }

        emu->sound_timer--;

        // If we just reached 0, stop the beep
        if (emu->sound_timer == 0)
        {
            audio_stop_beep();
        }
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
