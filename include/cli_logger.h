/**
 * @file cli_logger.h
 * @brief Header file for CLI logging functions.
 *
 * This file contains the declarations of functions used for logging
 * informational, warning, error, and timestamped messages to the CLI.
 */

#ifndef CLI_LOGGER_H
#define CLI_LOGGER_H

#include <stdarg.h>
#include <stdbool.h>

// ANSI color codes for terminal
#define RESET_COLOR "\033[0m"      // Reset to default color
#define COLOR_INFO "\033[1;34m"    // Blue
#define COLOR_WARNING "\033[1;33m" // Yellow
#define COLOR_ERROR "\033[1;31m"   // Red
#define COLOR_DEBUG "\033[0;32m"   // Green

/**
 * @brief Prints an informational message to the CLI.
 *
 * @param format Format string.
 * @param ... Additional arguments.
 */
void print_info(const char *format, ...);

/**
 * @brief Prints a warning message to the CLI.
 *
 * @param format Format string.
 * @param ... Additional arguments.
 */
void print_warning(const char *format, ...);

/**
 * @brief Prints an error message to the CLI.
 *
 * @param format Format string.
 * @param ... Additional arguments.
 */
void print_error(const char *format, ...);

/**
 * @brief Prints a debug message to the CLI.
 *
 * @param format Format string.
 * @param ... Additional arguments.
 */
void print_debug(const char *format, ...);

/**
 * @brief Prints a timestamped message to the CLI.
 *
 * @param format Format string.
 * @param ... Additional arguments.
 */
void print_timestamped(const char *format, ...);

#endif // CLI_LOGGER_H
