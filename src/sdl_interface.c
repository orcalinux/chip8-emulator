/**
 * @file sdl_interface.c
 * @brief Implementation of SDL interface functions for CHIP-8.
 */

#include <stdio.h>
#include <stdlib.h>
#include "sdl_interface.h"

bool sdl_init(sdl_t *sdl, const config_t *config)
{
    //
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

    // Clear the renderer
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255); // black RGBA
    SDL_RenderClear(sdl->renderer);                      // clear the renderer
    SDL_RenderPresent(sdl->renderer);                    // present the renderer

    return true;
}

bool set_config_from_args(config_t *config, const int argc, const char *argv[])
{
    *config = (config_t){
        .window_width = 640,
        .window_height = 320};

    if (argc >= 4)
    {
        char *endptr = NULL;
        long w = strtol(argv[2], &endptr, 10);
        if (endptr == argv[2] || w <= 0)
        {
            fprintf(stderr, "Invalid window width: %s. Using default 640.\n", argv[2]);
            w = 640;
        }

        endptr = NULL;
        long h = strtol(argv[3], &endptr, 10);
        if (endptr == argv[3] || h <= 0)
        {
            fprintf(stderr, "Invalid window height: %s. Using default 320.\n", argv[3]);
            h = 320;
        }

        *config = (config_t){
            .window_width = (uint32_t)w,
            .window_height = (uint32_t)h};
    }

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
            // fill out for all hex keys 0x0..0xF
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
