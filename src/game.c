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

#include "definitions.h"
#include "game.h"
#include "str_trim.h"
#include "console.h"
 
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
    int wordlist_size = 3000;  // Large enough to avoid multiple reallocations
    char **wordlist = (char**) malloc(sizeof(char*) * wordlist_size);
    if(wordlist == NULL) {
        fprintf(stderr, "\x1b[31;1mError allocating wordlist memory: %s\x1b[0m\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Read individual lines from wordlist file using manual character reading
    int i = 0;
    char *line_buffer = NULL;
    size_t buffer_size = 0;
    size_t line_pos = 0;
    int ch;
    
    // Initialize buffer
    buffer_size = 256;
    line_buffer = (char*) malloc(buffer_size);
    if(!line_buffer) {
        fprintf(stderr, "\x1b[31;1mError allocating line buffer: %s\x1b[0m\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    while((ch = fgetc(file)) != EOF) {
        // Expand buffer if needed
        if(line_pos >= buffer_size - 1) {
            buffer_size *= 2;
            char *temp = (char*) realloc(line_buffer, buffer_size);
            if(!temp) {
                fprintf(stderr, "\x1b[31;1mError reallocating line buffer: %s\x1b[0m\n", strerror(errno));
                free(line_buffer);
                exit(EXIT_FAILURE);
            }
            line_buffer = temp;
        }
        
        if(ch == '\n') {
            // End of line - process it
            line_buffer[line_pos] = '\0';
            
            if(line_buffer[0] == '\0') {
                line_pos = 0;
                continue;           // Skip empty lines
            }
            if(line_buffer[0] == '#') {
                line_pos = 0;
                continue;            // Skip comments
            }

        size_t len = strlen(line_buffer);
        char *line_trimmed = (char*) malloc(sizeof(char) * (len + 1));
        len = trim_whitespace(line_trimmed, len + 1, line_buffer);

        char word[WORD_LENGTH + 1]; // Buffer for the word
        memset(word, 0, sizeof(word));  // Clear the buffer
        strncpy(word, line_trimmed, WORD_LENGTH);
        word[WORD_LENGTH] = '\0';

        // Skip words that are not exactly WORD_LENGTH characters
        if(strlen(word) != WORD_LENGTH) {
            free(line_trimmed);
            continue;
        }

        int is_word = 1;        // Control variable to check if word is valid
        for(int j = 0; j < WORD_LENGTH; j++) {
            if(word[j] >= 'a' && word[j] <= 'z') {
                word[j] = word[j] - 'a' + 'A';       // Convert to uppercase
            }
            if(word[j] < 'A' || word[j] > 'Z') is_word = 0;             // Check if word is valid
        }

        if(is_word){
            // Reallocate memory for wordlist if needed BEFORE adding the word
            if(i >= wordlist_size) {
                wordlist_size *= 2; // Double the size
                char **temp = (char**) realloc(wordlist, sizeof(char*) * wordlist_size);
                if(temp == NULL) {
                    fprintf(stderr, "\x1b[31;1mError reallocating wordlist memory: %s\x1b[0m\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                wordlist = temp;
            }
            
            wordlist[i] = (char*) malloc(sizeof(char) * 6);
            strcpy(wordlist[i], word);
            i++;
        }
        
        // Free the temporary trimmed line
        free(line_trimmed);
        
        // Reset for next line
        line_pos = 0;
        } else {
            // Add character to line buffer
            line_buffer[line_pos++] = (char)ch;
        }
    }

    // Free the line buffer
    free(line_buffer);

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
    while(game->turn <= MAX_TURNS) {
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
    for(size_t i = 0; i < game->wordlist_size; i++) {
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
            // EOF reached (Ctrl+C or pipe ended), exit gracefully
            game_end(game);
            exit(EXIT_SUCCESS);
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = '\0';
        
        // Trim whitespace
        trim_whitespace(trimmed, sizeof(trimmed), input);
        
        // Check if word is 5 letters
        if (strlen(trimmed) != WORD_LENGTH) {
            fprintf(stdout, "\x1b[1A");         // Move up
            fprintf(stdout, "\x1b[2K");         // Clear line
            fprintf(stdout, "\x1b[31;1mError: Word must be %d characters long. Try again.\x1b[0m\n", WORD_LENGTH);
            continue;
        }
        
        // Copy to game->guess and convert to uppercase and validate input against alphabet
        memset(game->guess, 0, sizeof(game->guess));        // Clear the guess buffer
        strncpy(game->guess, trimmed, WORD_LENGTH);
        int invalid_input = 0;
        for (int i = 0; i < WORD_LENGTH; i++) {
            // Convert to uppercase (if char is lowercase)
            if (game->guess[i] >= 'a' && game->guess[i] <= 'z') {
                game->guess[i] = game->guess[i] - 'a' + 'A';
            }

            // Check if character is a valid uppercase letter
            if (game->guess[i] < 'A' || game->guess[i] > 'Z') {
                invalid_input = 1;
                break;
            }
        }

        if (invalid_input) {
            fprintf(stdout, "\x1b[1A");         // Move up
            fprintf(stdout, "\x1b[2K\r");       // Clear line
            fprintf(stdout, "\x1b[31;1mError: Word must only contain English letters. Try again.\x1b[0m\n");
            continue;
        }

        // Check if the guessed word is in the wordlist
        int found = 0;
        for (size_t i = 0; i < game->wordlist_size; i++) {
            if (strcmp(game->guess, game->wordlist[i]) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            fprintf(stdout, "\x1b[1A");         // Move up
            fprintf(stdout, "\x1b[2K\r");       // Clear line
            fprintf(stdout, "\x1b[31;1mError: Not a word. Try again.\x1b[0m\n");
            continue;
        }

        // If everything is fine, break the loop
        break;
    }
}
 
void game_print_guess(game_t *game) {
    // Move up a line
    fprintf(stdout, "\x1b[1A");

    // Clear line and move back to the beginning of the line
    fprintf(stdout, "\x1b[2K\r");

    // Print guess beginning
    fprintf(stdout, "Turn %d: ", game->turn);

    // Print guess letters, default to incorrect annotation
    //-----------------------------------------------------

    for(int i = 0; i < WORD_LENGTH; i++) {
        fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;7m%c\x1b[0m", game->guess[i]);
    }

    // Go back to the beginning of the word
    fprintf(stdout, "\x1b[5D");

    // Array to count letters in the word
    int word_counts[ALPHABET_SIZE] = {0};

    // Count letters in the word to guess
    for(int i = 0; i < WORD_LENGTH; i++) {
        word_counts[game->word[i] - 'A']++;
    }

    // Annotate correct placement guesses
    for(int i = 0; i < WORD_LENGTH; i++) {
        if(game->guess[i] == game->word[i]) {
            // Print letter in green background
            fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;2m%c\x1b[0m", game->guess[i]);
            word_counts[game->guess[i] - 'A']--;        // Decrease word letter count
        }
        else {
            // Move cursor forward to the next letter
            fprintf(stdout, "\x1b[1C");
        }
    }

    // Go back to the beginning of the word
    fprintf(stdout, "\x1b[5D");

    // Annotate incorrect placement guesses
    for(int i = 0; i < WORD_LENGTH; i++) {
        if(game->guess[i] != game->word[i]) {
            // Check if the letter is in the word
            if(word_counts[game->guess[i] - 'A'] > 0) {
                // Print letter in yellow background
                fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;3m%c\x1b[0m", game->guess[i]);
                word_counts[game->guess[i] - 'A']--;    // Decrease word letter count
            } else {
                // Print letter in gray background (incorrect letter)
                fprintf(stdout, "\x1b[38;5;0m\x1b[48;5;7m%c\x1b[0m", game->guess[i]);
            }
        }
        else {
            // Move cursor forward to the next letter (already printed in green)
            fprintf(stdout, "\x1b[1C");
        }
    }

    // Print a new line and reset the colors
    fprintf(stdout, "\x1b[0m\n");
}

void game_print_result(game_t *game) {
    if(game->win) {
        fprintf(stdout, "\n\x1b[38;5;2mYou won! You guessed the word %s in %d turns!\x1b[0m\n", game->word, game->turn);
    }
    else {
        fprintf(stdout, "\n\x1b[38;5;1mYou lost! The word was %s!\x1b[0m\n", game->word);
    }
}
