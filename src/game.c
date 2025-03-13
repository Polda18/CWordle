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

void game_free(char **wordlist, int size) {
    for(int j = 0; j < size; j++) {
        free(wordlist[j]);
    }
    free(wordlist);
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

        for(int j = 0; j < 5; j++) {
            word[j] &= 0x5f;                    // Convert to uppercase
        }

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

    // Check if wordlist is empty
    if(i == 0) {
        fprintf(stderr, "\x1b[31;1mWordlist is empty\x1b[0m\n");
        exit(EXIT_FAILURE);
    }

    // Close wordlist file
    fclose(file);

    // Select random word from wordlist
    srand(time(NULL));
    int random = rand() % i;
    strcpy(game->word, wordlist[random]);

    // Free allocated memory
    game_free(wordlist, i);
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
 
void game_print_prompt(game_t *game) {
    char input[100];
    char trimmed[100];
    
    while (1) {
        fprintf(stdout, "Turn %d: Please enter your guess> ", game->turn);
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "\x1b[31;1mError reading input\x1b[0m\n");
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
            fprintf(stdout, "\x1b[1A");     // Move up
            fprintf(stdout, "\x1b[2K");     // Clear line
            fprintf(stdout, "\x1b[31;1mError: Word must only contain letters. Try again.\x1b[0m\n");
            continue;
        }
        
        break;
    }
}
 
void game_print_guess(game_t *game) {
    // Move up a line
    fprintf(stdout, "\x1b[1A");

    // Clear line
    fprintf(stdout, "\x1b[2K");

    // Print guess
    fprintf(stdout, "Turn %d: %s", game->turn, game->guess);

    // Annotate guess
    fprintf(stdout, "\x1b[5D");         // Move left by 5 characters
    for(int i = 0; i < 5; i++) {
        if(game->guess[i] == game->word[i]) {
            fprintf(stdout, "\x1b[1;42m\x1b[2;30m%c", game->guess[i]);        // Green annotation
        }
        else if(strchr(game->word, game->guess[i]) != NULL) {
            fprintf(stdout, "\x1b[1;43m\x1b[2;30m%c", game->guess[i]);        // Yellow annotation
        }
        else {
            fprintf(stdout, "\x1b[2;47m\x1b[2;30m%c", game->guess[i]);        // Gray annotation
        }
    }

    // Print a new line and reset the colors
    fprintf(stdout, "\x1b[0m\n");
}

void game_print_result(game_t *game) {
    if(game->win) {
        fprintf(stdout, "\n\x1b[1;32mYou won! You guessed the word %s in %d turns!\n", game->word, game->turn);
    }
    else {
        fprintf(stdout, "\n\x1b[1;31mYou lost! The word was %s!\n", game->word);
    }
}
