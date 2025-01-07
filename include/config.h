#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct
{
    uint32_t window_width;  /**< Width of the SDL window */
    uint32_t window_height; /**< Height of the SDL window */
    uint32_t fg_color;      /**< Foreground color (white) */
    uint32_t bg_color;      /**< Background color (black) */
} config_t;

#endif /* CONFIG_H */
