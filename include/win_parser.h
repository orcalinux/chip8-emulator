#ifndef WIN_PARSER_H
#define WIN_PARSER_H

#ifdef _WIN32

/**
 * @brief Global custom parser index for Windows.
 *
 * By maintaining a single index, we ensure that both the display parser
 * and audio parser consume arguments in sync, just like 'optind' on Unix.
 */
extern int g_win_optind;

#endif // _WIN32

#endif // WIN_PARSER_H
