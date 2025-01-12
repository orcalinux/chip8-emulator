/**
 * @file config.c
 * @brief Unified config for display + audio + ROM path.
 *
 * This file delegates Windows logic to parse_config_windows
 * and Unix logic to parse_config_unix.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "cli_logger.h"

#ifdef _WIN32
#include "win_parser.h"
#else
#include <getopt.h>
#include <unistd.h>
#endif

void print_usage(const char *prog_name, bool to_stderr)
{
    FILE *output = to_stderr ? stderr : stdout;

    fprintf(output,
            "Usage: %s [options] <path_to_rom>\n\n"
            "Options (Display):\n"
            "  -w, --width <width>        Window width (default: 640)\n"
            "  -h, --height <height>      Window height (default: 320)\n"
            "  -s, --scale <scale>        Scale factor (default: 10)\n"
            "  -f, --fg <fg_color>        Foreground color (hex, default: 0xFFFFFFFF)\n"
            "  -b, --bg <bg_color>        Background color (hex, default: 0x00000000)\n\n"
            "Options (Audio):\n"
            "  -A, --audio <on|off>       Enable or disable audio (default: on)\n"
            "  -W, --wav <path>           Path to beep sound file (default: assets/beep.wav)\n"
            "  -V, --vol <volume>         Set audio volume (0-128, default: 128)\n\n",
            prog_name);
}

/* Forward declarations of OS-specific parse logic */
#ifdef _WIN32
static bool parse_config_windows(app_config_t *config, int argc, char *argv[]);
#else
static bool parse_config_unix(app_config_t *config, int argc, char *argv[]);
#endif

bool parse_config(app_config_t *config, int argc, char *argv[])
{
    // Initialize default display
    config->display_cfg.window_width = 640;
    config->display_cfg.window_height = 320;
    config->display_cfg.fg_color = 0xFFFFFFFF;
    config->display_cfg.bg_color = 0x00000000;
    config->display_cfg.scale_factor = 10;

    // Initialize default audio
    config->audio_cfg.enabled = true;
    strncpy(config->audio_cfg.wav_path, "assets/beep.wav",
            sizeof(config->audio_cfg.wav_path) - 1);
    config->audio_cfg.wav_path[sizeof(config->audio_cfg.wav_path) - 1] = '\0';
    config->audio_cfg.volume = 128;

    // ROM path default
    config->rom_path[0] = '\0';

#ifdef _WIN32
    return parse_config_windows(config, argc, argv);
#else
    return parse_config_unix(config, argc, argv);
#endif
}

#ifdef _WIN32
/*-----------------------------------------------------------
 *   WINDOWS-SPECIFIC PARSER
 *----------------------------------------------------------*/
static bool parse_config_windows(app_config_t *config, int argc, char *argv[])
{
    // Start from the first real argument
    g_win_optind = 1;

    while (g_win_optind < argc)
    {
        const char *arg = argv[g_win_optind];

        // Display flags
        if ((strcmp(arg, "-w") == 0 || strcmp(arg, "--width") == 0) && (g_win_optind + 1 < argc))
        {
            config->display_cfg.window_width = atoi(argv[++g_win_optind]);
        }
        else if ((strcmp(arg, "-h") == 0 || strcmp(arg, "--height") == 0) && (g_win_optind + 1 < argc))
        {
            config->display_cfg.window_height = atoi(argv[++g_win_optind]);
        }
        else if ((strcmp(arg, "-s") == 0 || strcmp(arg, "--scale") == 0) && (g_win_optind + 1 < argc))
        {
            config->display_cfg.scale_factor = atoi(argv[++g_win_optind]);
            config->display_cfg.window_width = 64 * config->display_cfg.scale_factor;
            config->display_cfg.window_height = 32 * config->display_cfg.scale_factor;
        }
        else if ((strcmp(arg, "-f") == 0 || strcmp(arg, "--fg") == 0) && (g_win_optind + 1 < argc))
        {
            config->display_cfg.fg_color = strtoul(argv[++g_win_optind], NULL, 16);
        }
        else if ((strcmp(arg, "-b") == 0 || strcmp(arg, "--bg") == 0) && (g_win_optind + 1 < argc))
        {
            config->display_cfg.bg_color = strtoul(argv[++g_win_optind], NULL, 16);
        }
        // Audio flags
        else if ((strcmp(arg, "-A") == 0 || strcmp(arg, "--audio") == 0) && (g_win_optind + 1 < argc))
        {
            const char *val = argv[++g_win_optind];
            if (strcmp(val, "off") == 0 || strcmp(val, "0") == 0)
                config->audio_cfg.enabled = false;
            else
                config->audio_cfg.enabled = true;
        }
        else if ((strcmp(arg, "-W") == 0 || strcmp(arg, "--wav") == 0) && (g_win_optind + 1 < argc))
        {
            strncpy(config->audio_cfg.wav_path, argv[++g_win_optind],
                    sizeof(config->audio_cfg.wav_path) - 1);
            config->audio_cfg.wav_path[sizeof(config->audio_cfg.wav_path) - 1] = '\0';
        }
        else if ((strcmp(arg, "-V") == 0 || strcmp(arg, "--vol") == 0) && (g_win_optind + 1 < argc))
        {
            int vol = atoi(argv[++g_win_optind]);
            if (vol < 0)
                vol = 0;
            if (vol > 128)
                vol = 128;
            config->audio_cfg.volume = vol;
        }
        // Unknown or leftover
        else if (arg[0] == '-')
        {
            // Unknown option
            print_error("Invalid option: %s\n", arg);
            return false;
        }
        else
        {
            // We assume it's the ROM path
            strncpy(config->rom_path, arg, sizeof(config->rom_path) - 1);
            config->rom_path[sizeof(config->rom_path) - 1] = '\0';
            g_win_optind++;
            break; // Stop parsing once we get the ROM
        }

        g_win_optind++;
    }

    // If we STILL have no ROM path but there’s leftover
    if (config->rom_path[0] == '\0' && g_win_optind < argc)
    {
        strncpy(config->rom_path, argv[g_win_optind], sizeof(config->rom_path) - 1);
        config->rom_path[sizeof(config->rom_path) - 1] = '\0';
    }

    // Ensure ROM path is not empty
    if (config->rom_path[0] == '\0')
    {
        print_error("Error: No ROM file specified.\n");
        return false;
    }

    return true;
}
#else
/*-----------------------------------------------------------
 *   UNIX-SPECIFIC PARSER (getopt_long)
 *----------------------------------------------------------*/
static bool parse_config_unix(app_config_t *config, int argc, char *argv[])
{
    static struct option long_opts[] = {
        // Display
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"scale", required_argument, NULL, 's'},
        {"fg", required_argument, NULL, 'f'},
        {"bg", required_argument, NULL, 'b'},

        // Audio
        {"audio", required_argument, NULL, 'A'},
        {"wav", required_argument, NULL, 'W'},
        {"vol", required_argument, NULL, 'V'},
        {NULL, 0, NULL, 0}};

    // Reset or ensure fresh parse
    extern int optind;
    optind = 1; // start from the first real arg

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "w:h:s:f:b:A:W:V:", long_opts, &option_index)) != -1)
    {
        switch (opt)
        {
        // Display
        case 'w':
            config->display_cfg.window_width = atoi(optarg);
            break;
        case 'h':
            config->display_cfg.window_height = atoi(optarg);
            break;
        case 's':
            config->display_cfg.scale_factor = atoi(optarg);
            config->display_cfg.window_width = 64 * config->display_cfg.scale_factor;
            config->display_cfg.window_height = 32 * config->display_cfg.scale_factor;
            break;
        case 'f':
            config->display_cfg.fg_color = strtoul(optarg, NULL, 16);
            break;
        case 'b':
            config->display_cfg.bg_color = strtoul(optarg, NULL, 16);
            break;

        // Audio
        case 'A':
        {
            if (strcmp(optarg, "off") == 0 || strcmp(optarg, "0") == 0)
                config->audio_cfg.enabled = false;
            else
                config->audio_cfg.enabled = true;
            break;
        }
        case 'W':
        {
            strncpy(config->audio_cfg.wav_path, optarg,
                    sizeof(config->audio_cfg.wav_path) - 1);
            config->audio_cfg.wav_path[sizeof(config->audio_cfg.wav_path) - 1] = '\0';
            break;
        }
        case 'V':
        {
            int vol = atoi(optarg);
            if (vol < 0)
                vol = 0;
            if (vol > 128)
                vol = 128;
            config->audio_cfg.volume = vol;
            break;
        }
        default:
            return false;
        }
    }

    // leftover argument for ROM
    if (optind < argc)
    {
        strncpy(config->rom_path, argv[optind], sizeof(config->rom_path) - 1);
        config->rom_path[sizeof(config->rom_path) - 1] = '\0';
        optind++;
    }

    if (config->rom_path[0] == '\0')
    {
        print_error("Error: No ROM file specified.\n");
        return false;
    }

    return true;
}
#endif
