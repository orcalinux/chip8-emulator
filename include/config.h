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

/**
 * @brief Default configuration values for the CHIP-8 emulator.
 */
#define CONFIG_DEFAULT_WINDOW_WIDTH 640           /**< Default width of the SDL window */
#define CONFIG_DEFAULT_WINDOW_HEIGHT 320          /**< Default height of the SDL window */
#define CONFIG_DEFAULT_FG_COLOR 0xFFFFFFFF        /**< Default foreground color (white) */
#define CONFIG_DEFAULT_BG_COLOR 0x00000000        /**< Default background color (black) */
#define CONFIG_DEFAULT_SCALE_FACTOR 10            /**< Default scale factor for enlarging the display */
#define CONFIG_DEFAULT_WAV_PATH "assets/beep.wav" /**< Default path to the beep .wav file */
#define CONFIG_DEFAULT_VOLUME 128                 /**< Default audio volume (0-128) */

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
    uint8_t padding[4];     /**< Padding to align to 8 bytes */
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

#ifdef __cplusplus
extern "C"
{
#endif

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
