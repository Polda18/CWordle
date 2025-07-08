/*******************************************************
 * Wordle game in console
 * 
 * Author: Marek Poláček (Polda18)
 * Version: v1.0
 * Date: 12.03.2025
 * License: GNU GPLv3
 * 
 * File: retcode.h
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Return codes for the program
// These are used to indicate the success or failure of the program
// EXIT_SUCCESS is returned when the program completes successfully
// EXIT_FAILURE is returned when the program encounters an error
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

// Alphabet size
#define ALPHABET_SIZE   26

// Word length
#define WORD_LENGTH     5

// Maximum number of turns
#define MAX_TURNS       6

#endif
