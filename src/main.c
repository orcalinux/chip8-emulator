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
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "chip8.h"
#include "cli_logger.h"
#include "sdl_interface.h"
#include "config.h"
#include "audio.h"
#ifdef _WIN32
#include "win_parser.h" // If you're using a custom Windows parser index
#endif

// Simple text rendering helper
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255}; // White
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface)
    {
        print_error("Failed to render text: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

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
        print_error("Error: Failed to init chip8.\n");
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

    // 6) Initialize SDL_ttf for text rendering
    if (TTF_Init() == -1)
    {
        print_error("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Load a font
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/nerd-fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 28);
    if (!font)
    {
        print_error("Failed to load font: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // 7) Initialize audio (if enabled)
    if (app_cfg.audio_cfg.enabled)
    {
        if (!audio_init(app_cfg.audio_cfg.wav_path))
        {
            print_error("Audio initialization failed.\n");
            return EXIT_FAILURE;
        }
        // Set global volume for SDL_mixer
        Mix_Volume(-1, app_cfg.audio_cfg.volume);
    }

    // 8) Main loop
    bool running = true;
    bool previous_frame[64 * 32] = {false};

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

            // If the emulator is paused, wait for a key press
            if (emu.state == CHIP8_PAUSED)
                continue;
        }

        // If still running, do one CPU cycle
        if (emu.state == CHIP8_RUNNING)
        {
            chip8_cycle(&emu);                             // Execute instructions
            chip8_timers_tick_60hz(&emu);                  // Update timers at ~60 Hz
            sdl_update_screen(&sdl, &emu, previous_frame); // Render if needed
        }

        // Small delay to avoid maxing out CPU
        SDL_Delay(1);
    }

    // 9) Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    sdl_cleanup(&sdl);

    if (app_cfg.audio_cfg.enabled)
    {
        audio_cleanup();
    }

    return EXIT_SUCCESS;
}
