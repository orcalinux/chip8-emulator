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
 * @brief Function type for CHIP-8 opcode handlers.
 *
 * Each handler takes a pointer to the emulator state (chip8_t)
 * and a decoded instruction (chip8_instr_t).
 */
typedef void (*chip8_opcode_handler_t)(chip8_t *, chip8_instr_t);

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

/* --------------------------------------------------------------------------
   Opcode Handlers
   -------------------------------------------------------------------------- */

/**
 * @brief Handler for 0x00E0: CLS (Clear Screen).
 */
static void handle_cls(chip8_t *emu, chip8_instr_t instr)
{
    (void)instr; // Not used

    // Clear the entire 64x32 display (bool array)
    memset(emu->display, false, sizeof(emu->display));
    emu->pc += 2;
}

/**
 * @brief Handler for 0x00EE: RET (Return from subroutine).
 */
static void handle_ret(chip8_t *emu, chip8_instr_t instr)
{
    (void)instr; // Not used

    if (emu->sp > 0)
    {
        emu->sp--;
        emu->pc = emu->stack[emu->sp];
        // Do NOT increment PC after returning
    }
    else
    {
        print_warning("Stack underflow on RET");
        emu->pc += 2; // Prevent getting stuck by skipping RET opcode
    }
}

/**
 * @brief Sub-dispatch for 0x0___ instructions.
 */
static void handle_0xxx(chip8_t *emu, chip8_instr_t instr)
{
    switch (instr.opcode)
    {
    case 0x00E0: // CLS
        handle_cls(emu, instr);
        break;
    case 0x00EE: // RET
        handle_ret(emu, instr);
        break;
    default:
        // Some interpreters handle 0NNN calls to RCA 1802 programs.
        print_warning("Unimplemented 0x0NNN opcode: 0x%04X", instr.opcode);
        emu->pc += 2;
        break;
    }
}

/**
 * @brief Handler for 0x1NNN: JP addr
 */
static void handle_jp(chip8_t *emu, chip8_instr_t instr)
{
    emu->pc = instr.nnn;
}

/**
 * @brief Handler for 0x2NNN: CALL addr
 */
static void handle_call(chip8_t *emu, chip8_instr_t instr)
{
    if (emu->sp < 16) // CHIP-8 has a stack depth of 16
    {
        emu->stack[emu->sp++] = emu->pc;
        emu->pc = instr.nnn;
    }
    else
    {
        print_warning("Stack overflow on CALL 0x%03X", instr.nnn);
        // Decide how to handle overflow (e.g., stop emulator)
        emu->state = CHIP8_STOPPED;
    }
}

/**
 * @brief Handler for 0x3XNN: SE Vx, NN (skip next if Vx == NN).
 */
static void handle_se_vx_kk(chip8_t *emu, chip8_instr_t instr)
{
    if (emu->V[instr.x] == instr.kk)
        emu->pc += 4;
    else
        emu->pc += 2;
}

/**
 * @brief Handler for 0x4XNN: SNE Vx, NN (skip next if Vx != NN).
 */
static void handle_sne_vx_kk(chip8_t *emu, chip8_instr_t instr)
{
    if (emu->V[instr.x] != instr.kk)
        emu->pc += 4;
    else
        emu->pc += 2;
}

/**
 * @brief Handler for 0x5XY0: SE Vx, Vy (skip next if Vx == Vy).
 */
static void handle_se_vx_vy(chip8_t *emu, chip8_instr_t instr)
{
    if (emu->V[instr.x] == emu->V[instr.y])
        emu->pc += 4;
    else
        emu->pc += 2;
}

/**
 * @brief Handler for 0x6XNN: LD Vx, NN.
 */
static void handle_ld_vx_kk(chip8_t *emu, chip8_instr_t instr)
{
    emu->V[instr.x] = instr.kk;
    emu->pc += 2;
}

/**
 * @brief Handler for 0x7XNN: ADD Vx, NN (Vx += NN).
 */
static void handle_add_vx_kk(chip8_t *emu, chip8_instr_t instr)
{
    emu->V[instr.x] += instr.kk;
    emu->pc += 2;
}

/**
 * @brief Sub-dispatch for 0x8XY_ instructions.
 */
static void handle_8xxx(chip8_t *emu, chip8_instr_t instr)
{
    switch (instr.n)
    {
    case 0x0: // LD Vx, Vy
        emu->V[instr.x] = emu->V[instr.y];
        break;
    case 0x1: // OR Vx, Vy
        emu->V[instr.x] |= emu->V[instr.y];
        break;
    case 0x2: // AND Vx, Vy
        emu->V[instr.x] &= emu->V[instr.y];
        break;
    case 0x3: // XOR Vx, Vy
        emu->V[instr.x] ^= emu->V[instr.y];
        break;
    case 0x4: // ADD Vx, Vy (with carry)
    {
        uint16_t sum = emu->V[instr.x] + emu->V[instr.y];
        emu->V[0xF] = (sum > 0xFF) ? 1 : 0; // Carry flag
        emu->V[instr.x] = (uint8_t)(sum & 0xFF);
        break;
    }
    case 0x5: // SUB Vx, Vy (with borrow)
    {
        emu->V[0xF] = (emu->V[instr.x] >= emu->V[instr.y]) ? 1 : 0;
        emu->V[instr.x] -= emu->V[instr.y];
        break;
    }
    case 0x6: // SHR Vx
    {
        emu->V[0xF] = emu->V[instr.x] & 0x1; // Least significant bit
        emu->V[instr.x] >>= 1;
        break;
    }
    case 0x7: // SUBN Vx, Vy
    {
        emu->V[0xF] = (emu->V[instr.y] >= emu->V[instr.x]) ? 1 : 0;
        emu->V[instr.x] = emu->V[instr.y] - emu->V[instr.x];
        break;
    }
    case 0xE: // SHL Vx
    {
        emu->V[0xF] = (emu->V[instr.x] & 0x80) >> 7; // Most significant bit
        emu->V[instr.x] <<= 1;
        break;
    }
    default:
        print_warning("Unknown 0x8 opcode: 0x%04X", instr.opcode);
        break;
    }

    emu->pc += 2;
}

/**
 * @brief Handler for 0x9XY0: SNE Vx, Vy (skip next if Vx != Vy).
 */
static void handle_sne_vx_vy(chip8_t *emu, chip8_instr_t instr)
{
    if (emu->V[instr.x] != emu->V[instr.y])
        emu->pc += 4;
    else
        emu->pc += 2;
}

/**
 * @brief Handler for 0xANNN: LD I, NNN
 */
static void handle_ld_i_nnn(chip8_t *emu, chip8_instr_t instr)
{
    emu->I = instr.nnn;
    emu->pc += 2;
}

/**
 * @brief Handler for 0xBNNN: JP V0, NNN
 */
static void handle_jp_v0_nnn(chip8_t *emu, chip8_instr_t instr)
{
    emu->pc = (emu->V[0] + instr.nnn) & 0xFFF; // Ensure PC wraps to 12 bits
}

/**
 * @brief Handler for 0xCXNN: RND Vx, NN (random & NN).
 */
static void handle_rnd_vx_kk(chip8_t *emu, chip8_instr_t instr)
{
    uint8_t rnd = (uint8_t)(rand() & 0xFF);
    emu->V[instr.x] = rnd & instr.kk;
    emu->pc += 2;
}

/**
 * @brief Handler for 0xDXYN: DRW Vx, Vy, N
 *
 * Draws N rows of 8 bits from memory[I].
 * Each bit toggles (XOR) the display pixel at (x+col, y+row).
 * VF = 1 if any pixel flipped from set (true) to unset (false).
 */
static void handle_drw_vx_vy_n(chip8_t *emu, chip8_instr_t instr)
{
    uint8_t x = emu->V[instr.x] % 64; // Fixed screen width for CHIP-8
    uint8_t y = emu->V[instr.y] % 32; // Fixed screen height for CHIP-8
    uint8_t height = instr.n;

    print_debug("Drawing sprite at (%d, %d) with height %d, I=0x%03X", x, y, height, emu->I);

    emu->V[0xF] = 0; // Reset collision flag

    for (uint8_t row = 0; row < height; row++)
    {
        // Prevent reading beyond memory bounds
        if (emu->I + row >= CHIP8_MEMORY_SIZE)
        {
            print_warning("Sprite row out of memory bounds: I=0x%03X, row=%d", emu->I, row);
            break;
        }

        uint8_t sprite_byte = emu->memory[emu->I + row];
        print_debug("Sprite row %d: 0x%02X", row, sprite_byte);

        for (uint8_t col = 0; col < 8; col++)
        {
            uint8_t pixel = (sprite_byte >> (7 - col)) & 1;
            uint16_t dst_x = x + col;
            uint16_t dst_y = y + row;

            if (dst_x >= 64 || dst_y >= 32)
            {
                print_debug("Skipping pixel out of bounds at (%d, %d)", dst_x, dst_y);
                continue;
            }

            uint16_t screen_idx = dst_y * 64 + dst_x;
            uint8_t *screen_pixel = &emu->display[screen_idx];

            // Debug: Print pixel status before XOR
            print_debug("Pixel before: %d, Sprite pixel: %d at (%d, %d)", *screen_pixel, pixel, dst_x, dst_y);

            if (*screen_pixel && pixel)
                emu->V[0xF] = 1;

            *screen_pixel ^= pixel;

            // Debug: Print pixel status after XOR
            print_debug("Pixel after: %d at (%d, %d)", *screen_pixel, dst_x, dst_y);
        }
    }

    emu->pc += 2;
}

/**
 * @brief Sub-dispatch for 0xE___ instructions (EX9E, EXA1).
 */
static void handle_exxx(chip8_t *emu, chip8_instr_t instr)
{
    switch (instr.kk)
    {
    case 0x9E: // SKP Vx
        if (emu->keys[emu->V[instr.x]])
            emu->pc += 4;
        else
            emu->pc += 2;
        break;
    case 0xA1: // SKNP Vx
        if (!emu->keys[emu->V[instr.x]])
            emu->pc += 4;
        else
            emu->pc += 2;
        break;
    default:
        print_warning("Unknown 0xE opcode: 0x%04X", instr.opcode);
        emu->pc += 2;
        break;
    }
}

/**
 * @brief Sub-dispatch for 0xF___ instructions (FX07, FX0A, FX15, etc.).
 */
static void handle_fxxx(chip8_t *emu, chip8_instr_t instr)
{
    switch (instr.kk)
    {
    case 0x07: // LD Vx, DT
        emu->V[instr.x] = emu->delay_timer;
        emu->pc += 2;
        break;
    case 0x0A: // LD Vx, K (wait for keypress)
    {
        bool key_pressed = false;
        for (int i = 0; i < 16; i++)
        {
            if (emu->keys[i])
            {
                emu->V[instr.x] = (uint8_t)i;
                key_pressed = true;
                break;
            }
        }
        if (!key_pressed)
        {
            // Retry same opcode until a key is pressed
            return; // Do NOT pc += 2
        }
        emu->pc += 2;
        break;
    }
    case 0x15: // LD DT, Vx
        emu->delay_timer = emu->V[instr.x];
        emu->pc += 2;
        break;
    case 0x18: // LD ST, Vx
        emu->sound_timer = emu->V[instr.x];
        emu->pc += 2;
        break;
    case 0x1E: // ADD I, Vx
        emu->I += emu->V[instr.x];
        emu->pc += 2;
        break;
    case 0x29: // LD F, Vx (I = location of sprite for digit in Vx)
        emu->I = (uint16_t)(emu->V[instr.x] * 5);
        emu->pc += 2;
        break;
    case 0x33: // LD B, Vx (store BCD of Vx)
    {
        uint8_t value = emu->V[instr.x];
        if (emu->I + 2 < CHIP8_MEMORY_SIZE)
        {
            emu->memory[emu->I + 0] = (uint8_t)(value / 100);
            emu->memory[emu->I + 1] = (uint8_t)((value / 10) % 10);
            emu->memory[emu->I + 2] = (uint8_t)(value % 10);
        }
        else
        {
            print_warning("BCD write out of memory bounds: I=0x%03X", emu->I);
        }
        emu->pc += 2;
        break;
    }
    case 0x55: // LD [I], V0..Vx
    {
        for (int i2 = 0; i2 <= instr.x; i2++)
        {
            if (emu->I + i2 < CHIP8_MEMORY_SIZE)
                emu->memory[emu->I + i2] = emu->V[i2];
            else
                print_warning("LD [I], Vx out of memory bounds: I+%d=0x%03X", i2, emu->I + i2);
        }
        // Some interpreters modify I here; ours does not
        emu->pc += 2;
        break;
    }
    case 0x65: // LD V0..Vx, [I]
    {
        for (int i2 = 0; i2 <= instr.x; i2++)
        {
            if (emu->I + i2 < CHIP8_MEMORY_SIZE)
                emu->V[i2] = emu->memory[emu->I + i2];
            else
                print_warning("LD Vx, [I] out of memory bounds: I+%d=0x%03X", i2, emu->I + i2);
        }
        emu->pc += 2;
        break;
    }
    default:
        print_warning("Unknown 0xF opcode: 0x%04X", instr.opcode);
        emu->pc += 2;
        break;
    }
}

/* --------------------------------------------------------------------------
   LOOKUP TABLE - DISPATCH by the high nibble
   -------------------------------------------------------------------------- */

static chip8_opcode_handler_t opcode_table[16] = {
    [0x0] = handle_0xxx,        // 0xxx opcodes -> sub-dispatch
    [0x1] = handle_jp,          // 0x1NNN
    [0x2] = handle_call,        // 0x2NNN
    [0x3] = handle_se_vx_kk,    // 0x3XNN
    [0x4] = handle_sne_vx_kk,   // 0x4XNN
    [0x5] = handle_se_vx_vy,    // 0x5XY0
    [0x6] = handle_ld_vx_kk,    // 0x6XNN
    [0x7] = handle_add_vx_kk,   // 0x7XNN
    [0x8] = handle_8xxx,        // 0x8XY_ -> sub-dispatch
    [0x9] = handle_sne_vx_vy,   // 0x9XY0
    [0xA] = handle_ld_i_nnn,    // 0xANNN
    [0xB] = handle_jp_v0_nnn,   // 0xBNNN
    [0xC] = handle_rnd_vx_kk,   // 0xCXNN
    [0xD] = handle_drw_vx_vy_n, // 0xDXYN
    [0xE] = handle_exxx,        // 0xEX__
    [0xF] = handle_fxxx         // 0xFX__
};

/* --------------------------------------------------------------------------
   Emulator Lifecycle
   -------------------------------------------------------------------------- */

bool chip8_init(chip8_t *emu)
{
    memset(emu, 0, sizeof(chip8_t));
    emu->state = CHIP8_RUNNING;

    // Program counter starts at 0x200 (standard for most CHIP-8)
    emu->pc = CHIP8_ROM_ENTRY_POINT;

    // Load standard fontset into memory starting at 0x000
    memcpy(emu->memory, chip8_fontset, sizeof(chip8_fontset));

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
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    if (file_size < 0)
    {
        print_error("Error determining ROM file size.\n");
        fclose(fp);
        return false;
    }

    // Validate the file size against available memory space
    const size_t max_rom_size = CHIP8_MEMORY_SIZE - CHIP8_ROM_ENTRY_POINT; // 4096 - 512 = 3584
    if ((size_t)file_size > max_rom_size)
    {
        print_warning("ROM file too large: %ld bytes (max allowed: %zu bytes)", file_size, max_rom_size);
        fclose(fp);
        return false;
    }

    // Read the file into memory starting at 0x200
    size_t bytes_read = fread(&emu->memory[CHIP8_ROM_ENTRY_POINT], 1, (size_t)file_size, fp);
    fclose(fp);

    if (bytes_read != (size_t)file_size)
    {
        print_error("Failed to read entire ROM file: %s\n", filepath);
        return false;
    }

    print_info("Loaded ROM: %s (%ld bytes)", filepath, file_size);
    return true;
}

void chip8_cycle(chip8_t *emu)
{
    // Ensure PC is within memory bounds
    if (emu->pc + 1 >= CHIP8_MEMORY_SIZE)
    {
        print_error("Program Counter out of bounds: PC=0x%03X", emu->pc);
        emu->state = CHIP8_STOPPED;
        return;
    }

    // Fetch 2 bytes from memory
    uint16_t raw_opcode = (uint16_t)((emu->memory[emu->pc] << 8) | emu->memory[emu->pc + 1]);

    // Decode the opcode
    emu->current_instr = decode_opcode(raw_opcode);

    // Log the decoded instruction for debugging
    debug_log_instruction(emu);

    // Dispatch to the appropriate handler based on the high nibble
    uint8_t high_nibble = (uint8_t)((emu->current_instr.opcode & 0xF000) >> 12);
    if (opcode_table[high_nibble])
    {
        opcode_table[high_nibble](emu, emu->current_instr);
    }
    else
    {
        print_warning("Unknown opcode: 0x%04X at PC=0x%03X", emu->current_instr.opcode, emu->pc);
        emu->pc += 2; // Skip unknown opcode
    }
}

void chip8_timers_decrement(chip8_t *emu)
{
    if (emu->delay_timer > 0)
        emu->delay_timer--;

    if (emu->sound_timer > 0)
    {
        // If not already playing a beep, start the beep sound
        if (!audio_is_beep_playing())
            audio_play_beep_loop();

        emu->sound_timer--;

        // Stop the beep sound when the timer reaches zero
        if (emu->sound_timer == 0)
            audio_stop_beep();
    }
}

void chip8_timers_tick_60hz(chip8_t *emu)
{
    static uint64_t lastCount = 0;
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t freq = SDL_GetPerformanceFrequency();

    if (lastCount == 0)
        lastCount = now;

    double elapsed = (double)(now - lastCount) / (double)freq;
    double interval = 1.0 / 60.0;

    while (elapsed >= interval)
    {
        chip8_timers_decrement(emu);
        elapsed -= interval;
        lastCount += (uint64_t)(interval * freq);
    }
}

void debug_log_instruction(const chip8_t *emu)
{
    static bool debug_enabled = true;
    if (!debug_enabled)
        return;

    const chip8_instr_t *instr = &emu->current_instr;
    print_debug("PC: 0x%03X | Opcode: 0x%04X | x: %X | y: %X | kk: 0x%02X | nnn: 0x%03X | n: %X | I: 0x%03X\n",
                emu->pc, instr->opcode, instr->x, instr->y, instr->kk, instr->nnn, instr->n, emu->I);
}
