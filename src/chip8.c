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

/**
 * @brief Decodes the raw 16-bit opcode into a chip8_instr_t struct.
 *
 * @param opcode The raw 16-bit opcode fetched from memory.
 * @return A fully populated chip8_instr_t.
 */
static chip8_instr_t decode_opcode(uint16_t opcode)
{
    chip8_instr_t instr;
    instr.opcode = opcode;

    instr.nnn = opcode & 0x0FFF;    // Lower 12 bits
    instr.kk = opcode & 0x00FF;     // Lower 8 bits
    instr.x = (opcode >> 8) & 0x0F; // Bits 8..11
    instr.y = (opcode >> 4) & 0x0F; // Bits 4..7
    instr.n = opcode & 0x000F;      // Lowest nibble (bits 0..3)

    return instr;
}

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
    // 1. Fetch the raw 16-bit opcode
    uint16_t raw_opcode = (emu->memory[emu->pc] << 8) | emu->memory[emu->pc + 1];

    // 2. Decode the opcode into a chip8_instr_t
    chip8_instr_t instr = decode_opcode(raw_opcode);

    // 3. Execute the opcode.
    //    Typically, you check the highest nibble (instr.opcode & 0xF000),
    //    or you can structure your switch based on the high nibble and sub-nibbles.
    switch (instr.opcode & 0xF000)
    {
    case 0x1000: // JP addr (jump to nnn)
        emu->pc = instr.nnn;
        return; // Important: we don't increment pc here if we're jumping

    case 0x6000: // LD Vx, byte (Set Vx = kk)
        emu->V[instr.x] = instr.kk;
        emu->pc += 2;
        break;

    case 0x7000: // ADD Vx, byte (Set Vx = Vx + kk)
        emu->V[instr.x] += instr.kk;
        emu->pc += 2;
        break;

    default:
        print_warning("Unknown opcode: 0x%04X at PC=0x%03X\n", instr.opcode, emu->pc);
        emu->pc += 2; // skip unknown opcode
        break;
    }
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
