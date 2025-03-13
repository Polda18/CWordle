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

typedef struct {
    char word[6];               // Word to guess
    char guess[6];              // Player's guess
    int turn;                   // Current turn
    int win;                    // Win flag
    char **wordlist;            // Wordlist
    size_t wordlist_size;       // Wordlist size
} game_t;

typedef struct {
    char letter;                // Letter definition
    int count;                  // Letter count
    int guessed;                // Letter guessed
} letter_count_t;

typedef struct
{
    letter_count_t *letter_counters;    // Array of letter counters
    size_t len;                         // Array length
} counter_arr_t;


/**
 * Free the game wordlist allocator
 * 
 * @param wordlist Game descriptor
 * @param size Wordlist size
 */

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
 * Annotate guess
 * 
 * @param game Game descriptor
 * @param w_counters Word counters
 * @param g_counters Guess counters
 */
void game_annotate_guess(game_t *game, counter_arr_t *w_counters, counter_arr_t *g_counters);

/**
 * Print game result
 * 
 * @param game Game descriptor
 */
void game_print_result(game_t *game);

#endif
