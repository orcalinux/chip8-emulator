/**
 * @file config.c
 * @brief Implementation of the emulator configuration parser.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"

/**
 * If we're on Windows, we'll do a custom parser.
 * Otherwise, use getopt_long for Unix-like systems.
 */
#ifdef _WIN32
#define USE_CUSTOM_PARSER
static int optind = 1; // We'll reuse "optind" as a Windows custom parser index
#else
#include <getopt.h> // For getopt_long on Unix
#endif

void print_usage(const char *prog_name, bool to_stderr)
{
    FILE *output = to_stderr ? stderr : stdout;

    fprintf(output,
            "Usage: %s [options] <path_to_rom>\n"
            "Options:\n"
            "  -w, --width <width>        Window width\n"
            "  -h, --height <height>      Window height\n"
            "  -s, --scale <scale>        Scale factor\n"
            "  -f, --fg <fg_color>        Foreground color (hex)\n"
            "  -b, --bg <bg_color>        Background color (hex)\n"
            "      --help                 Show this help message\n",
            prog_name);
}

/**
 * @brief Parses command-line arguments for emulator config:
 *        On Windows -> custom parser
 *        On Unix    -> getopt_long
 */
bool sdl_parse_config_from_args(config_t *config, int argc, char *argv[])
{
    // 1) Set defaults
    config->window_width = 640;
    config->window_height = 320;
    config->fg_color = 0xFFFFFFFF;
    config->bg_color = 0x00000000;
    config->scale_factor = 10;
    config->rom_path[0] = '\0';

#ifdef USE_CUSTOM_PARSER
    // --------------------------------------------------
    // Windows: Custom Parser
    // --------------------------------------------------
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) && i + 1 < argc)
        {
            config->window_width = atoi(argv[++i]);
        }
        else if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0) && i + 1 < argc)
        {
            config->window_height = atoi(argv[++i]);
        }
        else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scale") == 0) && i + 1 < argc)
        {
            config->scale_factor = atoi(argv[++i]);
            config->window_width = 64 * config->scale_factor;
            config->window_height = 32 * config->scale_factor;
        }
        else if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fg") == 0) && i + 1 < argc)
        {
            config->fg_color = strtoul(argv[++i], NULL, 16);
        }
        else if ((strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bg") == 0) && i + 1 < argc)
        {
            config->bg_color = strtoul(argv[++i], NULL, 16);
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            // Show usage & exit
            print_usage(argv[0], false);
            exit(EXIT_SUCCESS);
        }
        else if (argv[i][0] != '-')
        {
            // Assume this is the ROM path
            strncpy(config->rom_path, argv[i], sizeof(config->rom_path) - 1);
            config->rom_path[sizeof(config->rom_path) - 1] = '\0';
            // Mark we found the ROM, so next time we come here, we skip
            optind = i + 1;
            break;
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0], true);
            return false;
        }
    }
#else
    // --------------------------------------------------
    // Unix: getopt_long
    // --------------------------------------------------
    static struct option long_options[] = {
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"scale", required_argument, NULL, 's'},
        {"fg", required_argument, NULL, 'f'},
        {"bg", required_argument, NULL, 'b'},
        {"help", no_argument, NULL, 0},
        {NULL, 0, NULL, 0}};

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "w:h:s:f:b:", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'w':
            config->window_width = atoi(optarg);
            break;
        case 'h':
            config->window_height = atoi(optarg);
            break;
        case 's':
            config->scale_factor = atoi(optarg);
            config->window_width = 64 * config->scale_factor;
            config->window_height = 32 * config->scale_factor;
            break;
        case 'f':
            config->fg_color = strtoul(optarg, NULL, 16);
            break;
        case 'b':
            config->bg_color = strtoul(optarg, NULL, 16);
            break;
        case 0: // long option with no short equivalent
            if (strcmp(long_options[option_index].name, "help") == 0)
            {
                print_usage(argv[0], false);
                exit(EXIT_SUCCESS);
            }
            break;
        default:
            print_usage(argv[0], true);
            return false;
        }
    }
#endif // USE_CUSTOM_PARSER

    // 2) Check if a ROM file is provided
    //    On Unix, 'optind' from getopt
    //    On Windows, we've stored it in 'optind' manually

    // If we didn't fill config->rom_path in Windows parser,
    // or if 'optind' points beyond the argument list on Unix:
#ifdef USE_CUSTOM_PARSER
    int local_optind = optind; // custom "optind"
#else
    int local_optind = optind; // real getopt optind
#endif

    if (config->rom_path[0] == '\0') // Windows case: no ROM found in loop
    {
        // Check if there's still an argument left
        if (local_optind < argc)
        {
            strncpy(config->rom_path, argv[local_optind], sizeof(config->rom_path) - 1);
            config->rom_path[sizeof(config->rom_path) - 1] = '\0';
        }
    }

    // Now if we STILL have no ROM, error out
    if (config->rom_path[0] == '\0')
    {
        fprintf(stderr, "Error: No ROM file specified.\n");
        print_usage(argv[0], true);
        return false;
    }

    return true;
}
