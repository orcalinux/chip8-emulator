/**
 * @file sdl_interface.h
 * @brief SDL-related functions to display CHIP-8 graphics and handle input.
 *
 * This header file declares functions for initializing SDL, rendering the
 */

#ifndef SDL_INTERFACE_H
#define SDL_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "config.h"
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

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes SDL, creates a window, renderer, and texture.
     *
     * @param sdl Pointer to sdl_t struct to be filled.
     * @param window_width The desired window width.
     * @param window_height The desired window height.
     * @return true on success, false on error.
     */
    bool sdl_init(sdl_t *sdl, const display_config_t *config);

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
     * @brief Update only if there's a change in the display array.
     *
     * @param sdl Pointer to sdl_t struct.
     * @param emu Pointer to chip8_t struct containing display state.
     * @param previous_frame A 64*32 bool array tracking previous state.
     */
    void sdl_update_screen(const sdl_t *sdl, const chip8_t *emu, bool *previous_frame);

    /**
     * @brief Cleans up and destroys SDL window, renderer, and texture.
     *
     * @param sdl Pointer to sdl_t struct to clean up.
     */
    void sdl_cleanup(sdl_t *sdl);

#ifdef __cplusplus
}
#endif

#endif /* SDL_INTERFACE_H */
