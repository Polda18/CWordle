/*******************************************************
 * Wordle game in console
 * 
 * Author: Marek Poláček (Polda18)
 * Version: v1.0
 * Date: 12.03.2025
 * License: GNU GPLv3
 * 
 * File: ansi_escapes.h
 */

// Windows
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "console.h"

#ifdef _WIN32
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
// Terminal states
static HANDLE stdout_handle;
static DWORD out_mode_init;

void setup_console(void) {
    DWORD out_mode = 0;
    stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(stdout_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error getting stdout handle: %s\n", strerror(errno));
        exit(GetLastError());
    }

    if(!GetConsoleMode(stdout_handle, &out_mode)) {
        fprintf(stderr, "Error getting console mode: %s\n", strerror(errno));
        exit(GetLastError());
    }

    out_mode_init = out_mode;

    // Enable ANSI escape sequences
    out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if(!SetConsoleMode(stdout_handle, out_mode)) {
        fprintf(stderr, "Error setting console mode: %s\n", strerror(errno));
        exit(GetLastError());
    }
}

void restore_console(void) {
    fprintf(stdout, "\x1b[0m");

    if(!SetConsoleMode(stdout_handle, out_mode_init)) {
        fprintf(stderr, "Error restoring console mode: %s\n", strerror(errno));
        exit(GetLastError());
    }
}
#else
void setup_console(void) {}

void restore_console(void) {
    fprintf(stdout, "\x1b[0m");
}
#endif
