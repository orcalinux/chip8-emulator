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

#include "chip8.h"
#include "sdl_interface.h"
#include "config.h"

// Simple text rendering helper
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255}; // White
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface)
    {
        fprintf(stderr, "Failed to render text: %s\n", TTF_GetError());
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
    // 1) Prepare a config_t structure
    config_t config;
    // Parse command-line args, fill config (width, height, scale, colors, rom_path)
    if (!sdl_parse_config_from_args(&config, argc, argv))
    {
        return EXIT_FAILURE; // If parsing failed or ROM not specified
    }

    // 2) Initialize CHIP-8 emulator state
    chip8_t emu;
    chip8_init(&emu);    // E.g., sets pc=0x200, clears memory, etc.
    emu.config = config; // Copy config into emulator

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

    // 5) Initialize SDL_ttf for text rendering
    if (TTF_Init() == -1)
    {
        fprintf(stderr, "SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Load a font
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/nerd-fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 28);
    if (!font)
    {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // 6) Main loop
    bool running = true;
    bool previous_frame[64 * 32] = {false};

    while (running)
    {
        // Declare the SDL_Event variable 'event' here
        SDL_Event event;
        // Process all pending events
        while (SDL_PollEvent(&event))
        {
            sdl_handle_event(&emu, &event);

            // If the emulator's state was changed to STOPPED, exit
            if (emu.state == CHIP8_STOPPED)
            {
                running = false;
            }
        }

        // If the emulator is still running, do one CPU cycle
        if (emu.state == CHIP8_RUNNING)
        {
            chip8_cycle(&emu);                             // Execute instructions
            chip8_timers_tick_60hz(&emu);                  // Update timers ~60 Hz
            sdl_update_screen(&sdl, &emu, previous_frame); // Draw if needed
        }

        // Some small delay to avoid maxing out CPU
        SDL_Delay(1);
    }

    // 7) Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    sdl_cleanup(&sdl);

    return EXIT_SUCCESS;
}
