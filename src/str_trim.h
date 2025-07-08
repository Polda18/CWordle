/*******************************************************
 * Wordle game in console
 * 
 * Author: Marek Poláček (Polda18)
 * Version: v1.0
 * Date: 12.03.2025
 * License: GNU GPLv3
 * 
 * File: str_trim.h
 */

#ifndef STR_TRIM_H
#define STR_TRIM_H

#include <stddef.h>

/**
 * Stores the trimmed input string into the given output buffer, which must be
 * large enough to store the result.  If it is too small, the output is
 * truncated.
 * 
 * @param out Output buffer
 * @param len Input buffer length
 * @param str String to strip whitespace from
 */
size_t trim_whitespace(char *out, size_t len, const char *str);

#endif
