/*******************************************************
 * Wordle game in console
 * 
 * Author: Marek Poláček (Polda18)
 * Version: v1.0
 * Date: 12.03.2025
 * License: GNU GPLv3
 * 
 * File: game.h
 */

#ifndef GAME_H
#define GAME_H

#include "definitions.h"

// Structure to hold game state
typedef struct {
    char word[WORD_LENGTH + 1];     // Word to guess
    char guess[WORD_LENGTH + 1];    // Player's guess
    int turn;                       // Current turn
    int win;                        // Win flag
    char **wordlist;                // Wordlist
    size_t wordlist_size;           // Wordlist size
} game_t;

/**
 * Initialize game
 * 
 * @param game Game descriptor
 */
void game_init(game_t *game);

/**
 * Main game loop
 * 
 * @param game Game descriptor
 */
void game_loop(game_t *game);

/**
 * End game
 * 
 * @param game Game descriptor
 */
void game_end(game_t *game);

/**
 * Print game prompt
 * 
 * @param game Game descriptor
 */
void game_print_prompt(game_t *game);

/**
 * Print game guess
 * 
 * @param game Game descriptor
 */
void game_print_guess(game_t *game);

/**
 * Print game result
 * 
 * @param game Game descriptor
 */
void game_print_result(game_t *game);

#endif
