# Wordle in console

This weird little program is actually a game you can play in your Terminal.
This is my implementation of Wordle in C. The game is portable. Just place
the contents of the ZIP file anywhere you want, perhaps on your flash drive,
and you can play on any system with that flash drive connected.

This game is available for both Windows and Linux. Linux version should work
on MacOS as well, but that hasn't been tested.

## How to play?

1. Extract this ZIP archive anywhere you like.
   Make sure the extracted archive includes the wordlist.txt
   This file is essential for this game to work!
2. Run the "wordle" program from the extracted directory.
   This is important to load the wordlist.txt file correctly.
3. Play the game and test your luck. You can use upper case or lower
   case letters. All words are 5 letter words, the game picks one
   by random. Guessed words are output using upper case lettering.
4. If you want to play again, run the game executable again.

## Editing the word list

- As you may know, there's wordlist.txt file bundled with this game.
  This file is the list of words that this program can use. This file
  can be edited. To edit this file, open it in a text editor and
  add or remove your words.
- You can also add comments to the file. Comment is any line that
  starts with "#". All words shall be 5-letter words.
