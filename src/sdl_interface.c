/**
 * @file sdl_interface.c
 * @brief Implementation of SDL interface functions for CHIP-8.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "sdl_interface.h"

/**
 * @brief Extracts RGBA components from a 32-bit color value.
 *
 * @param color 32-bit color value.
 * @param r Pointer to red component.
 * @param g Pointer to green component.
 * @param b Pointer to blue component.
 * @param a Pointer to alpha component.
 */
static void extract_rgba(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
{
    *r = (color >> 24) & 0xFF;                   // Extract red component
    *g = (color >> 16) & 0xFF;                   // Extract green component
    *b = (color >> 8) & 0xFF;                    // Extract blue component
    *a = (color & 0xFF) ? (color & 0xFF) : 0xFF; // Default to opaque if alpha is 0
}

bool sdl_init(sdl_t *sdl, const config_t *config)
{
    // Initialize SDL with everything enabled (audio, video, etc.)
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create a window
    sdl->window = SDL_CreateWindow("CHIP-8 Emulator",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   config->window_width, config->window_height,
                                   SDL_WINDOW_SHOWN);

    if (!sdl->window)
    {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create a renderer for the window
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer)
    {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create a texture to hold the display pixels
    sdl->texture = SDL_CreateTexture(sdl->renderer,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     64, 32);
    if (!sdl->texture)
    {
        SDL_Log("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Extract background color from config
    uint8_t r, g, b, a;
    extract_rgba(config->bg_color, &r, &g, &b, &a);

    // Clear the renderer using bg_color
    SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);
    SDL_RenderClear(sdl->renderer);
    SDL_RenderPresent(sdl->renderer);

    return true;
}

void sdl_render(const sdl_t *sdl, const chip8_t *emu)
{
    // The chip8_t's display is 64x32. We'll convert it into ARGB8888 pixels.
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; i++)
    {
        // If display[i] is true, pixel is white; otherwise black
        bool pixel_on = emu->display[i];
        pixels[i] = pixel_on ? 0xFFFFFFFF : 0xFF000000;
    }

    // Update the texture with our pixel buffer
    SDL_UpdateTexture(sdl->texture, NULL, pixels, 64 * sizeof(uint32_t));

    // Render
    SDL_RenderClear(sdl->renderer);
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
}

void sdl_handle_event(chip8_t *emu, const SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
        case SDLK_1:
            emu->keys[0x1] = true;
            break;
        case SDLK_2:
            emu->keys[0x2] = true;
            break;
            // ...
            // TODO: fill out for all hex keys 0x0..0xF
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
        case SDLK_1:
            emu->keys[0x1] = false;
            break;
        case SDLK_2:
            emu->keys[0x2] = false;
            break;
            // ...
        }
    }
}

void sdl_update_screen(const sdl_t *sdl, const chip8_t *emu, bool *previous_frame)
{
    bool changed = false;

    for (int i = 0; i < 64 * 32; i++)
    {
        if (emu->display[i] != previous_frame[i])
        {
            changed = true;
            break;
        }
    }

    if (!changed)
    {
        // No difference, skip rendering
        return;
    }

    // If changed, build new pixel buffer
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; i++)
    {
        previous_frame[i] = emu->display[i]; // Update previous_frame
        pixels[i] = emu->display[i] ? emu->config.fg_color : emu->config.bg_color;
    }

    SDL_UpdateTexture(sdl->texture, NULL, pixels, 64 * sizeof(uint32_t));
    SDL_RenderClear(sdl->renderer);
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
}

void sdl_cleanup(sdl_t *sdl)
{
    if (sdl->texture)
    {
        SDL_DestroyTexture(sdl->texture);
        sdl->texture = NULL;
    }

    if (sdl->renderer)
    {
        SDL_DestroyRenderer(sdl->renderer);
        sdl->renderer = NULL;
    }

    if (sdl->window)
    {
        SDL_DestroyWindow(sdl->window);
        sdl->window = NULL;
    }

    SDL_Quit();
}
