# Legacy of the Fiend

Built for the 2025 edition of the 7 Day Roguelike Challenge

99 levels of dungeon

12 kinds of monsters

1 fiendish final boss

and several cats to pet along the way

Controls:

A - Pick up items

B - Cancel ranged attack / wait one turn

C - Initiate/confirm ranged attacks

Start - Toggle extended message log

Directions - Walk/Melee Attack/Move ranged targeting reticle


## Building the ROM

Build it with the "make" command

Import art/music by adding it to a folder inside the "assets" directory, and then run "make import" to update generated project files.

If you have pulled and built the GameTankEmulator repo in an adjacent folder, run "make emulate" to test the game.

src/main.c contains the code that will run after the console powers on

## Build requirements:

* cc65 https://cc65.github.io/

  * recommended to use a snapshot build or build from source as their last tagged release and apt package are outdated

* NodeJS https://nodejs.org/

* Zopfli https://github.com/google/zopfli

* GNU Make

## Testing requirements:

* GameTank Emulator https://github.com/clydeshaffer/GameTankEmulator
