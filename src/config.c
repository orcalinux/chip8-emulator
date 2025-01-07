#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"

void print_usage(const char *prog_name, bool to_stderr)
{
    FILE *output = to_stderr ? stderr : stdout;

    fprintf(output, "Usage: %s [options] <path_to_rom>\n"
                    "Options:\n"
                    "  -w, --width <width>        Window width\n"
                    "  -h, --height <height>      Window height\n"
                    "  -s, --scale <scale>        Scale factor\n"
                    "  -f, --fg <fg_color>        Foreground color (hex)\n"
                    "  -b, --bg <bg_color>        Background color (hex)\n"
                    "      --help                 Show this help message\n",
            prog_name);
}

bool sdl_parse_config_from_args(config_t *config, int argc, char *argv[])
{
    // 1) Set defaults
    config->window_width = 640;
    config->window_height = 320;
    config->fg_color = 0xFFFFFFFF;
    config->bg_color = 0x00000000;
    config->scale_factor = 10;

    // 2) Define long options
    static struct option long_options[] = {
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"scale", required_argument, NULL, 's'},
        {"fg", required_argument, NULL, 'f'},
        {"bg", required_argument, NULL, 'b'},
        {"help", no_argument, NULL, 0}, // Long-only --help
        {NULL, 0, NULL, 0}              // End marker
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "w:h:s:f:b:", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'w': // Width
            config->window_width = atoi(optarg);
            break;
        case 'h': // Height
            config->window_height = atoi(optarg);
            break;
        case 's': // Scale
            config->scale_factor = atoi(optarg);
            config->window_width = 64 * config->scale_factor;
            config->window_height = 32 * config->scale_factor;
            break;
        case 'f': // Foreground color
            config->fg_color = strtoul(optarg, NULL, 16);
            break;
        case 'b': // Background color
            config->bg_color = strtoul(optarg, NULL, 16);
            break;
        case 0: // --help
            if (strcmp("help", long_options[option_index].name) == 0)
            {
                print_usage(argv[0], false); // Print to stdout for help
                exit(EXIT_SUCCESS);
            }
            break;
        default:
            print_usage(argv[0], true); // Print to stderr for errors
            return false;
        }
    }

    // Ensure a ROM file path is specified after options
    if (optind >= argc)
    {
        fprintf(stderr, "Error: No ROM file specified.\n");
        print_usage(argv[0], true); // Error case, print to stderr
        return false;
    }

    return true;
}
