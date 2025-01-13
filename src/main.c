/**
 * @file main.c
 * @brief Entry point for the CHIP-8 emulator using SDL.
 *
 * This file contains the main function that initializes the CHIP-8 emulator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "chip8.h"
#include "cli_logger.h"
#include "sdl_interface.h"
#include "config.h"
#include "audio.h"
#ifdef _WIN32
#include "win_parser.h"
#endif

int main(int argc, char *argv[])
{
    // 1) Create a single config structure for display, audio, and ROM
    app_config_t app_cfg;
    memset(&app_cfg, 0, sizeof(app_cfg));

#ifdef _WIN32
    // Initialize the custom parser index for Windows
    g_win_optind = 1;
#endif

    // 2) Parse command-line arguments
    //    If this fails, print usage and exit
    if (!parse_config(&app_cfg, argc, argv))
    {
        print_error("Failed to parse configuration.\n");
        print_usage(argv[0], true);
        return EXIT_FAILURE;
    }

    // 3) Initialize CHIP-8 emulator state
    chip8_t emu;
    if (!chip8_init(&emu))
    {
        print_error("Error: Failed to initialize CHIP-8 emulator.\n");
        return EXIT_FAILURE;
    }

    // Copy display config into emulator
    emu.config = app_cfg.display_cfg;

    // 4) Load the ROM
    if (!chip8_load_program(&emu, app_cfg.rom_path))
    {
        print_error("Failed to load ROM: %s\n", app_cfg.rom_path);
        return EXIT_FAILURE;
    }

    // 5) Initialize SDL (window, renderer, texture)
    sdl_t sdl;
    if (!sdl_init(&sdl, &app_cfg.display_cfg))
    {
        print_error("SDL initialization failed.\n");
        return EXIT_FAILURE;
    }

    // 6) Initialize audio (if enabled)
    if (app_cfg.audio_cfg.enabled)
    {
        if (!audio_init(app_cfg.audio_cfg.wav_path))
        {
            print_error("Audio initialization failed.\n");
            sdl_cleanup(&sdl);
            return EXIT_FAILURE;
        }
        // Set global volume for SDL_mixer
        Mix_Volume(-1, app_cfg.audio_cfg.volume);
    }

    // 7) Main loop
    bool running = true;
    uint8_t previous_frame[64 * 32];
    memset(previous_frame, 0, sizeof(previous_frame));

    while (running)
    {
        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            sdl_handle_event(&emu, &event);

            // If the emulator's state changes to STOPPED, exit
            if (emu.state == CHIP8_STOPPED)
            {
                running = false;
            }

            // If the emulator is paused, skip emulation cycle
            if (emu.state == CHIP8_PAUSED)
                break;
        }

        // If still running, do one CPU cycle
        if (emu.state == CHIP8_RUNNING)
        {
            chip8_cycle(&emu);                             // Execute instructions
            chip8_timers_tick_60hz(&emu);                  // Update timers at ~60 Hz
            sdl_update_screen(&sdl, &emu, previous_frame); // Render if needed
        }

        // Small delay to control emulation speed and prevent high CPU usage
        SDL_Delay(1);
    }

    // 8) Cleanup
    sdl_cleanup(&sdl);

    if (app_cfg.audio_cfg.enabled)
    {
        audio_cleanup();
    }

    return EXIT_SUCCESS;
}
