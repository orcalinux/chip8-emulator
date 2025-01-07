/**
 * @file main.c
 * @brief Entry point for the CHIP-8 emulator using SDL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "chip8.h"
#include "sdl_interface.h"

int main(int argc, char *argv[])
{
    // 1) Prepare a config_t structure
    config_t config;
    // Pass the command-line args so we can override default window size (640x320)
    if (!sdl_parse_config_from_args(&config, argc, argv))
    {
        return EXIT_FAILURE;
    }

    // 2) Initialize CHIP-8 emulator state
    chip8_t emu;
    chip8_init(&emu);

    // Load the ROM from argv[1] if you haven't already
    // if (!chip8_load_program(&emu, argv[1]))
    // {
    //     fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
    //     return EXIT_FAILURE;
    // }

    // 3) Initialize SDL (window, renderer, texture)
    sdl_t sdl;
    if (!sdl_init(&sdl, &config))
    {
        fprintf(stderr, "SDL initialization failed.\n");
        return EXIT_FAILURE;
    }

    // 4) Main loop
    bool running = true;
    bool previous_frame[64 * 32] = {false};

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else
            {
                // sdl_handle_event(&emu, &event);
            }
        }

        // Perform one or more CHIP-8 CPU cycles, update timers, etc.
        // chip8_cycle(&emu);

        // Render the CHIP-8 display
        // sdl_render(&sdl, &emu);

        // Update timers at 60 Hz
        chip8_timers_tick_60hz(&emu);

        // Update screen only if the display has changed
        sdl_update_screen(&sdl, &emu, previous_frame);

        // Slight delay to prevent maxing out CPU
        SDL_Delay(1);
    }

    // 5) Cleanup
    sdl_cleanup(&sdl);

    return EXIT_SUCCESS;
}
