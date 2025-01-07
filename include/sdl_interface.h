/**
 * @file sdl_interface.h
 * @brief SDL-related functions to display CHIP-8 graphics and handle input.
 */

#ifndef SDL_INTERFACE_H
#define SDL_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"

/**
 * @struct sdl_t
 * @brief Holds pointers to SDL structures for rendering the CHIP-8 display.
 */
typedef struct
{
    SDL_Window *window;     /**< SDL window handle */
    SDL_Renderer *renderer; /**< SDL renderer handle */
    SDL_Texture *texture;   /**< SDL texture for the 64x32 display */
} sdl_t;

typedef struct
{
    uint32_t window_width;  /**< Width of the SDL window */
    uint32_t window_height; /**< Height of the SDL window */
    uint32_t fg_color;      /**< Foreground color (white) */
    uint32_t bg_color;      /**< Background color (black) */
} config_t;

/**
 * @brief Initializes SDL, creates a window, renderer, and texture.
 *
 * @param sdl Pointer to sdl_t struct to be filled.
 * @param window_width The desired window width.
 * @param window_height The desired window height.
 * @return true on success, false on error.
 */
bool sdl_init(sdl_t *sdl, const config_t *config);

/**
 * @brief Parses command-line arguments to set the window size.
 *
 * @param config Pointer to config_t struct to be filled.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return true on success, false on error.
 */
bool set_config_from_args(config_t *config, const int argc, const char *argv[]);

/**
 * @brief Renders the CHIP-8 display onto the SDL window.
 *
 * @param sdl Pointer to sdl_t struct.
 * @param emu Pointer to chip8_t struct containing the display state.
 */
void sdl_render(const sdl_t *sdl, const chip8_t *emu);

/**
 * @brief Updates the CHIP-8 key states from SDL events.
 *
 * @param emu Pointer to chip8_t struct (to update keys).
 * @param event SDL_Event to process.
 */
void sdl_handle_event(chip8_t *emu, const SDL_Event *event);

/**
 * @brief Cleans up and destroys SDL window, renderer, and texture.
 *
 * @param sdl Pointer to sdl_t struct to clean up.
 */
void sdl_cleanup(sdl_t *sdl);

#endif /* SDL_INTERFACE_H */
