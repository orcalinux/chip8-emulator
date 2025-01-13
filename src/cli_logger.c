/**
 * @file cli_logger.c
 * @brief Implementation of CLI logging functions.
 *
 * This file contains the definitions of functions used for logging
 * informational, warning, error, and timestamped messages to the CLI.
 */

#include "cli_logger.h"
#include <stdio.h>
#include <time.h>

void print_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf(COLOR_INFO "[INFO] " RESET_COLOR);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void print_warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf(COLOR_WARNING "[WARNING] " RESET_COLOR);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void print_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf(COLOR_DEBUG "[DEBUG] " RESET_COLOR);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void print_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf(COLOR_ERROR "[ERROR] " RESET_COLOR);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void print_timestamped(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    printf("[TIMESTAMP %s] ", time_str);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}
