#ifndef TILE_H
#define TILE_H

#include <iostream>
#include <string>

// Defines the character value of each colour
#define RED 'R'
#define YELLOW 'Y'
#define DARK_BLUE 'B'
#define LIGHT_BLUE 'L'
#define BLACK 'U'
#define FIRST_PLAYER 'F'
#define NO_TILE '.'

// Defines the notation used for the empty mosaic spots
#define BOARD_RED 'r'
#define BOARD_YELLOW 'y'
#define BOARD_DARK_BLUE 'b'
#define BOARD_LIGHT_BLUE 'l'
#define BOARD_BLACK 'u'

// Number of different coloured tiles
#define NUM_COLOURS 5

// Holds all possible values for a tile
enum Colour { Red = 'R', Yellow = 'Y', Dark_Blue = 'B', Light_Blue = 'L', Black = 'U', first_player = 'F', no_tile = '.', board_Red = 'r', board_Yellow = 'y', board_Dark_Blue = 'b', board_Light_Blue = 'l', board_Black = 'u', unrecognised = '?'};

class Tile {
  public:
    Tile();
    Tile(Colour colour);
    Tile(Colour colour, Tile* next);
    Tile(Tile* other);
    Tile* getNext();
    Colour getColour();
    void setNext(Tile* tile);
    void setColour(Colour newColour);
    // Returns the character to be printed for the tile
    char printColour();

  private:
    Colour colour;
    Tile* next;
};

#endif //TILE_H
