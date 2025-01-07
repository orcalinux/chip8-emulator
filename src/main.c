/**
 * @file main.c
 * @brief Entry point for the CHIP-8 emulator using SDL.
 *
 * This file contains the main function that initializes the CHIP-8 emulator,
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "chip8.h"
#include "sdl_interface.h"
#include "config.h"

int main(int argc, char *argv[])
{
    // 1) Prepare a config_t structure
    config_t config;
    // Parse command-line args, fill config (width, height, scale, colors, rom_path)
    if (!sdl_parse_config_from_args(&config, argc, argv))
    {
        return EXIT_FAILURE; // If parsing failed or ROM not specified
    }

    // 2) Initialize CHIP-8 emulator state
    chip8_t emu;
    chip8_init(&emu);

    // Optionally, copy config to emu if you want the emulator to own it
    emu.config = config;

    // 3) Load the ROM from config.rom_path
    if (!chip8_load_program(&emu, config.rom_path))
    {
        fprintf(stderr, "Failed to load ROM: %s\n", config.rom_path);
        return EXIT_FAILURE;
    }

    // 4) Initialize SDL (window, renderer, texture)
    sdl_t sdl;
    if (!sdl_init(&sdl, &config))
    {
        fprintf(stderr, "SDL initialization failed.\n");
        return EXIT_FAILURE;
    }

    // 5) Main loop
    bool running = true;
    bool previous_frame[64 * 32] = {false}; // For detecting display changes

    while (running)
    {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                // If you have key event handling, call sdl_handle_event
                // sdl_handle_event(&emu, &event);
            }
        }

        // Execute one or more CHIP-8 CPU cycles
        chip8_cycle(&emu);

        // Update timers at ~60 Hz
        chip8_timers_tick_60hz(&emu);

        // Refresh display if any pixels changed
        sdl_update_screen(&sdl, &emu, previous_frame);

        // Small delay to avoid 100% CPU usage
        SDL_Delay(1);
    }

    // 6) Cleanup
    sdl_cleanup(&sdl);
    return EXIT_SUCCESS;
}
