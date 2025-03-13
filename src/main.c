/*******************************************************
 * Wordle game in console
 * 
 * Author: Marek Poláček (Polda18)
 * Version: v1.0
 * Date: 12.03.2025
 * License: GNU GPLv3
 * 
 * File: main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "retcode.h"
#include "console.h"

int main(void) {
    setup_console();            // Setup console for use of ANSI escape sequences

    // Welcome message
    fprintf(stdout, "-----------------------------------------------------\n");
    fprintf(stdout, "Welcome to Wordle!\n");
    fprintf(stdout, "Author: Marek Poláček (Polda18)\n");
    fprintf(stdout, "Version: v1.0\n");
    fprintf(stdout, "Date: 12.03.2025\n");
    fprintf(stdout, "License: GNU GPLv3\n");
    fprintf(stdout, "-----------------------------------------------------\n\n");

    fprintf(stdout, "How to play:\n");
    fprintf(stdout, "1. Enter a 5-letter word.\n");
    fprintf(stdout, "2. You have 6 attempts to guess the word.\n");
    fprintf(stdout, "3. After each guess, the program will give you feedback.\n");
    fprintf(stdout, "   - \x1b[38;5;0m\x1b[48;5;2m  GREEN   \x1b[0m : Correct letter in the correct position.\n");
    fprintf(stdout, "   - \x1b[38;5;0m\x1b[48;5;3m  YELLOW  \x1b[0m : Correct letter in the wrong position.\n");
    fprintf(stdout, "   - \x1b[38;5;0m\x1b[48;5;7m   GRAY   \x1b[0m : Incorrect letter.\n");
    fprintf(stdout, "4. If you guess the word, you win!\n");
    fprintf(stdout, "5. If you run out of attempts, you lose.\n\n");

    // Initialize game
    game_t* game = (game_t*) malloc(sizeof(game_t));
    game_init(game);

    // Main game loop
    game_loop(game);

    // Exit after the game ended
    game_end(game);
    return EXIT_SUCCESS;
}
