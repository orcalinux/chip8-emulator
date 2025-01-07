/**
 * @file config.h
 * @brief Emulator configuration parser.
 *
 * This header file declares the functions and data structures for parsing
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/**
 * @struct config_t
 * @brief Holds emulator configuration settings.
 */
typedef struct
{
    uint32_t window_width;  /**< Width of the SDL window */
    uint32_t window_height; /**< Height of the SDL window */
    uint32_t fg_color;      /**< Foreground color (white) */
    uint32_t bg_color;      /**< Background color (black) */
    uint32_t scale_factor;  /**< Scale factor for enlarging the CHIP-8 display */
    char rom_path[256];     /**< Path to the CHIP-8 ROM file */
} config_t;

/**
 * @brief Print usage information for the emulator.
 * @param prog_name The name of the executable program.
 * @param to_stderr If true, print to stderr for errors. Otherwise, print to stdout.
 */
void print_usage(const char *prog_name, bool to_stderr);

/**
 * @brief Parses command-line arguments to set up the application configuration.
 *
 * This function processes the given command-line arguments and updates
 * the provided configuration object with the corresponding settings. It
 * extracts values related to the emulator's behavior, such as paths or runtime
 * options, so that the SDL interface can be initialized accordingly.
 *
 * @param config Pointer to a valid configuration structure that will be updated
 *               based on the parsed arguments.
 * @param argc   The number of arguments provided to the program.
 * @param argv   The array of argument strings passed to the program.
 *
 * @return True if the configuration settings were successfully parsed and
 *         applied; false otherwise.
 */
bool sdl_parse_config_from_args(config_t *config, int argc, char *argv[]);

#endif /* CONFIG_H */
