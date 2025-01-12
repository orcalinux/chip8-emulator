/**
 * @file config.h
 * @brief Unified configuration for the CHIP-8 emulator.
 *
 * This header declares a single structure that bundles display and audio
 * configuration, plus the ROM path. It also declares the function that
 * parses command-line arguments to populate this structure.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @struct display_config_t
     * @brief Holds display/window configuration parameters for the emulator.
     */
    typedef struct
    {
        uint32_t window_width;  /**< Width of the SDL window */
        uint32_t window_height; /**< Height of the SDL window */
        uint32_t fg_color;      /**< Foreground color (white) */
        uint32_t bg_color;      /**< Background color (black) */
        uint32_t scale_factor;  /**< Scale factor for enlarging the CHIP-8 display */
    } display_config_t;

    /**
     * @struct audio_config_t
     * @brief Holds audio-related configuration settings.
     */
    typedef struct
    {
        bool enabled;       /**< Whether audio is enabled */
        char wav_path[256]; /**< Path to the beep .wav file */
        int volume;         /**< Audio volume (0-128) */
    } audio_config_t;

    /**
     * @struct app_config_t
     * @brief Combines both display and audio configs, plus the ROM path.
     */
    typedef struct
    {
        display_config_t display_cfg; /**< Display-related configuration */
        audio_config_t audio_cfg;     /**< Audio-related configuration */
        char rom_path[256];           /**< Path to the CHIP-8 ROM file */
    } app_config_t;

    /**
     * @brief Parses all command-line arguments into a single app_config_t structure.
     *
     * This unified parser handles display flags, audio flags, and leftover arguments
     * (ROM path). If it encounters an unknown or invalid option, it returns false.
     *
     * @param config Pointer to app_config_t to populate.
     * @param argc   Number of command-line arguments.
     * @param argv   Array of command-line argument strings.
     *
     * @return true on success, false on failure (unknown flags, missing ROM, etc.).
     */
    bool parse_config(app_config_t *config, int argc, char *argv[]);

    /**
     * @brief Prints usage information for all available command-line options.
     *
     * @param prog_name Typically argv[0].
     * @param to_stderr If true, print to stderr; otherwise to stdout.
     */
    void print_usage(const char *prog_name, bool to_stderr);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
