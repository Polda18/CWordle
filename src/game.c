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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// Windows
#ifdef _WIN32
    #include <windows.h>
#endif

#include "game.h"
#include "retcode.h"
#include "str_trim.h"
#include "console.h"

void game_free(char **wordlist, int size) {
    
}
 
void game_init(game_t *game) {
    // Initialize game descriptor values
    game->turn = 1;
    game->guess[0] = '\0';
    game->word[0] = '\0';
    game->win = 0;

    // Open wordlist file
    FILE *file = fopen("wordlist.txt", "r");

    if(file == NULL) {
        fprintf(stderr, "\x1b[31;1mError opening wordlist file: %s\x1b[0m\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Allocate memory for wordlist
    int wordlist_size = 100;
    char **wordlist = (char**) malloc(sizeof(char*) * wordlist_size);
    if(wordlist == NULL) {
        fprintf(stderr, "\x1b[31;1mError allocating wordlist memory: %s\x1b[0m\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    char line[100];

    // Read individual lines from wordlist file
    int i = 0;
    while(fgets(line, sizeof(line), file) != NULL) {
        if(line[0] == '\n') continue;           // Skip empty lines

        if(line[sizeof(line) - 1] != '\n' && line[sizeof(line) - 1] != '\0') {
            // Line is too long, skip to the end of the line
            char bin;
            while((bin = fgetc(file)) != '\n' && bin != EOF);
        }
        line[strcspn(line, "\n")] = '\0';
        if(line[0] == '#') continue;            // Skip comments

        size_t len = strlen(line);
        char *line_trimmed = (char*) malloc(sizeof(char) * (len + 1));
        len = trim_whitespace(line_trimmed, len + 1, line);
        line_trimmed = (char*) realloc(line_trimmed, sizeof(char) * (len + 1));

        char word[6];
        strncpy(word, line_trimmed, 5);
        word[5] = '\0';

        int is_word = 1;        // Control variable to check if word is valid
        for(int j = 0; j < 5; j++) {
            if(word[j] >= 'a' && word[j] <= 'z') word[j] &= 0x5f;       // Convert to uppercase
            if(word[j] < 'A' || word[j] > 'Z') is_word = 0;             // Check if word is valid
        }

        if(is_word){
            wordlist[i] = (char*) malloc(sizeof(char) * 6);
            strcpy(wordlist[i], word);

            // Reallocate memory for wordlist if needed
            if(i % 100 == 0 && i != 0) {
                wordlist_size += 100;
                wordlist = (char**) realloc(wordlist, sizeof(char*) * wordlist_size);
                if(wordlist == NULL) {
                    fprintf(stderr, "\x1b[31;1mError reallocating wordlist memory: %s\x1b[0m\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            i++;
        }
    }

    // Save wordlist into memory
    game->wordlist_size = i;
    game->wordlist = wordlist;

    // Check if wordlist is empty
    if(i == 0) {
        fprintf(stderr, "\x1b[31;1mWordlist is empty\x1b[0m\n");
        game_end(game);
        exit(EXIT_FAILURE);
    }

    // Close wordlist file
    fclose(file);

    // Select random word from wordlist
    srand(time(NULL));
    int random = rand() % i;
    strcpy(game->word, wordlist[random]);
}
 
void game_loop(game_t *game) {
    while(game->turn <= 6) {
        game_print_prompt(game);
        game_print_guess(game);

        if(strcmp(game->guess, game->word) == 0) {
            game->win = 1;
            break;
        }
        game->turn++;
    }

    game_print_result(game);
}

void game_end(game_t *game) {
    // Free wordlist memory first
    for(int i = 0; i < game->wordlist_size; i++) {
        free(game->wordlist[i]);
    }
    free(game->wordlist);

    // Free game memory
    free(game);
    restore_console();
}
 
void game_print_prompt(game_t *game) {
    char input[100];
    char trimmed[100];
    
    while (1) {             // Endless loop to get user input
        fprintf(stdout, "Turn %d: Please enter your guess> ", game->turn);
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "\x1b[31;1mError reading input\x1b[0m\n");
            game_end(game);
            exit(EXIT_FAILURE);
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = '\0';
        
        // Trim whitespace
        trim_whitespace(trimmed, sizeof(trimmed), input);
        
        // Check if word is 5 letters
        if (strlen(trimmed) != 5) {
            fprintf(stdout, "\x1b[1A");         // Move up
            fprintf(stdout, "\x1b[2K");         // Clear line
            fprintf(stdout, "\x1b[31;1mError: Word must be 5 characters long. Try again.\x1b[0m\n");
            continue;
        }
        
        // Copy to game->guess and convert to uppercase
        strncpy(game->guess, trimmed, 5);
        int invalid_input = 0;
        for (int i = 0; i < 5; i++) {
            // Convert to uppercase using bitwise AND with 0x5F (if char is lowercase)
            if (game->guess[i] >= 'a' && game->guess[i] <= 'z') {
                game->guess[i] &= 0x5F;
            }

            if (game->guess[i] < 'A' || game->guess[i] > 'Z') {
                invalid_input = 1;
                break;
            }
        }

        if (invalid_input) {
            fprintf(stdout, "\x1b[1A");         // Move up
            fprintf(stdout, "\x1b[2K\r");       // Clear line
            fprintf(stdout, "\x1b[31;1mError: Word must only contain letters. Try again.\x1b[0m\n");
            continue;
        }
        
        break;
    }
}
 
void game_print_guess(game_t *game) {
    // Move up a line
    fprintf(stdout, "\x1b[1A");

    // Clear line and move back to the beginning of the line
    fprintf(stdout, "\x1b[2K\r");

    // Print guess
    fprintf(stdout, "Turn %d: ", game->turn);

    // Create word counters
    counter_arr_t *word_counters = (counter_arr_t*) malloc(sizeof(counter_arr_t));
    word_counters->len = 0;      // Initial size of zero
    word_counters->letter_counters = (letter_count_t*) malloc(5 * sizeof(letter_count_t));

    // Create guess counters
    counter_arr_t *guess_counters = (counter_arr_t*) malloc(sizeof(counter_arr_t));
    guess_counters->len = 0;      // Initial size of zero
    guess_counters->letter_counters = (letter_count_t*) malloc(5 * sizeof(letter_count_t));

    // Count word letters
    for(int i = 0; i < 5; i++) {
        char letter = game->word[i];            // Fetch letter from word

        // Check if letter is already in word_counters
        //--------------------------------------------

        // If word counter is empty, add letter
        if(word_counters->len == 0) {
            word_counters->letter_counters[word_counters->len].letter = letter;
            word_counters->letter_counters[word_counters->len].count = 1;
            word_counters->letter_counters[word_counters->len].guessed = -1;
            word_counters->len++;
            continue;
        }
        
        // If word counter is not empty, check if letter is already in word_counters, add if not
        for(int j = 0; j < word_counters->len; j++) {
            if(word_counters->letter_counters[j].letter == letter) {
                word_counters->letter_counters[j].count++;
                break;
            }

            if(j == word_counters->len - 1) {
                word_counters->letter_counters[word_counters->len].letter = letter;
                word_counters->letter_counters[word_counters->len].count = 1;
                word_counters->letter_counters[word_counters->len].guessed = -1;
                word_counters->len++;
                break;
            }
        }
    }

    // Count guess letters
    for(int i = 0; i < 5; i++) {
        char letter = game->guess[i];           // Fetch letter from guess

        // Check if letter is already in guess_counters
        //--------------------------------------------

        // If guess counter is empty, add letter
        if(guess_counters->len == 0) {
            guess_counters->letter_counters[guess_counters->len].letter = letter;
            guess_counters->letter_counters[guess_counters->len].count = 1;
            guess_counters->letter_counters[guess_counters->len].guessed = 0;
            guess_counters->len++;
            continue;
        }
        
        // If guess counter is not empty, check if letter is already in guess_counters, add if not    
        for(int j = 0; j < guess_counters->len; j++) {
            if(guess_counters->letter_counters[j].letter == letter) {
                guess_counters->letter_counters[j].count++;
                break;
            }

            if(j == guess_counters->len - 1) {
                guess_counters->letter_counters[guess_counters->len].letter = letter;
                guess_counters->letter_counters[guess_counters->len].count = 1;
                guess_counters->letter_counters[guess_counters->len].guessed = 0;
                guess_counters->len++;
                break;
            }
        }
    }

    // Annotate guess
    for(int i = 0; i < 5; i++) {
        int guess_letter_flag = 0;            // Flag to check for guess
        // Check if the letter is guessed correctly in wrong position
        for(int j = 0; j < guess_counters->len && !guess_letter_flag; j++) {
            // Check for the same letter in guess_counters and word_counters
            for(int k = 0; k < word_counters->len && !guess_letter_flag; k++) {
                if(guess_counters->letter_counters[j].letter == word_counters->letter_counters[k].letter) {
                    // Check if the letter is guessed correctly in correct position
                    if(game->guess[i] == game->word[i]) {
                        // Print letter in green background
                        fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;2m%c", game->guess[i]);
                    }
                    // Check if the letter is guessed correctly in wrong position
                    else if(game->guess[i] == word_counters->letter_counters[k].letter) {
                        // Check guess letter counts against actual letter counts
                        if(guess_counters->letter_counters[j].guessed > word_counters->letter_counters[k].count) {
                            // Out of letter guesses, print letter in gray background = Incorrect letter
                            fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;7m%c", game->guess[i]);
                        }
                        else {
                            // Print letter in yellow background
                            fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;3m%c", game->guess[i]);
                        }
                    }
                    else {
                        // Incorrect guess = gray background
                        fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;7m%c", game->guess[i]);
                    }

                    guess_counters->letter_counters[j].guessed++;           // Increment letter guesses count
                    guess_letter_flag = 1;                                  // Set guess_letter_flag to true
                }
                else if(k == word_counters->len - 1) {
                    // Out of letter = Incorrect guess = gray background
                    fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;7m%c", game->guess[i]);
                    guess_counters->letter_counters[j].guessed++;           // Increment letter guesses count
                    guess_letter_flag = 1;                                  // Set guess_letter_flag to true
                }
            }
        }
    }

    // Clean up
    //---------

    // Free memory for letter counters
    for(int i = 0; i < guess_counters->len; i++) {
        free(guess_counters->letter_counters);
    }

    // Free memory for word counters
    for(int i = 0; i < word_counters->len; i++) {
        free(word_counters->letter_counters);
    }

    // Free guess_counters and word_counters
    free(guess_counters);
    free(word_counters);

    // Print a new line and reset the colors
    fprintf(stdout, "\x1b[0m\n");
}


void game_annotate_guess(game_t *game, counter_arr_t *w_counters, counter_arr_t *g_counters) {

}

void game_print_result(game_t *game) {
    if(game->win) {
        fprintf(stdout, "\n\x1b[38;5;2mYou won! You guessed the word %s in %d turns!\x1b[0m\n", game->word, game->turn);
    }
    else {
        fprintf(stdout, "\n\x1b[38;5;1mYou lost! The word was %s!\x1b[0m\n", game->word);
    }
}
