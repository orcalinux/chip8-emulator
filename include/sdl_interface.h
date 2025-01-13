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
     * @brief Initializes the SDL interface for the CHIP-8 emulator.
     *
     * This function initializes SDL, creates a window, renderer, and texture,
     * and sets the initial render color based on the background color.
     *
     * @param sdl Pointer to the SDL interface structure to initialize.
     * @param config Pointer to the display configuration structure.
     * @return true if initialization is successful, false otherwise.
     */
    bool sdl_init(sdl_t *sdl, const display_config_t *config);

    /**
     * @brief Renders the CHIP-8 display to the SDL window.
     *
     * This function converts the CHIP-8's display buffer into ARGB8888 pixel data,
     * updates the SDL texture, and presents the rendered frame.
     *
     * @param sdl Pointer to the SDL interface structure.
     * @param emu Pointer to the CHIP-8 emulator structure.
     */
    void sdl_render(const sdl_t *sdl, const chip8_t *emu);

    /**
     * @brief Handles SDL events and updates the CHIP-8 emulator state accordingly.
     *
     * This function processes SDL events such as quitting the application, key presses,
     * and key releases, updating the emulator's state and key buffer as necessary.
     *
     * @param emu Pointer to the CHIP-8 emulator structure.
     * @param event Pointer to the SDL event structure.
     */
    void sdl_handle_event(chip8_t *emu, const SDL_Event *event);

    /**
     * @brief Updates the SDL texture and renders the display if changes are detected.
     *
     * This function compares the current display buffer with the previous frame to
     * determine if rendering is necessary. If changes are detected, it updates the
     * texture and presents the new frame.
     *
     * @param sdl Pointer to the SDL interface structure.
     * @param emu Pointer to the CHIP-8 emulator structure.
     * @param previous_frame Pointer to the buffer holding the previous frame's state.
     */
    void sdl_update_screen(const sdl_t *sdl, const chip8_t *emu, uint8_t *previous_frame);

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
