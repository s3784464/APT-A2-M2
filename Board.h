#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <array>

#include "Tile.h"
#include "Lid.h"

// Board size constants
#define NUM_MOSAIC_COLUMNS   5
#define NUM_ROWS  5
#define MAX_BROKEN_TILES  7

// Constants used to denote positions in tileColourCount array
#define COLOUR_COUNT_RED 0
#define COLOUR_COUNT_YELLOW 1
#define COLOUR_COUNT_DARK_BLUE 2
#define COLOUR_COUNT_LIGHT_BLUE 3
#define COLOUR_COUNT_BLACK 4

// Constants used to signify where in the mosaic a colour is to be placed
#define BOARD_RED_POSITION  2
#define BOARD_YELLOW_POSITION 1
#define BOARD_DARK_BLUE_POSITION  0
#define BOARD_LIGHT_BLUE_POSITION 4
#define BOARD_BLACK_POSITION  3

// Constants used to calculate points
#define POINTS_FULL_ROW 2
#define POINTS_FULL_COLUMN 7
#define POINTS_FULL_COLOUR 10
// Constant to denote special case of wanting to move tiles straight floor line
#define BROKEN_TILES_POSITION 6

typedef Tile Mosaic[NUM_ROWS][NUM_MOSAIC_COLUMNS];
typedef std::vector<Tile> Row;
typedef std::vector<Row> Rows;


class Board {
  public:
    Board();
    // Returns the tile at position in mosaic, input required to be inside bounds of mosaic.
    Tile getMosaicPosition(int i, int j);
    Rows getRows();
    Row getBrokenTiles();
    // Prints the storage rows, mosaic and broken tiles to std::cout
    void printBoard();
    // Initialises all the pieces of the board
    void initialiseBoard();
    // Initialises a single row
    Row initialiseRow(int length);
    // Sets the position in the mosaic to the input tile, input in bounds required
    void setMosaicPosition(int i, int j, Tile tile);
    // Adds an existing row into the Rows vector
    void addRow(Row row);
    // Adds tiles from a factory onto the board
    void addFactoryTilesToRow(Row row, int rowNumber, Lid* lid);
    // Adds a tile directly to the broken tiles (if there is space)
    void addBrokenTile(Tile tile, Lid* lid);
    // Checks whether a row selected by a user is part of a valid move
    bool validRowChoice(int rowNumber, Colour* colour);
    // Loads the Broken Tiles from a saved game
    void loadBrokenTile(Tile tile);
    // Checks whether the board contains the first player token
    bool hasFirstPlayerToken();
    // Ends the rounds and calculates points earned during round
    int endRound(Lid* lid);
    // Checks whether the conditions of ending the game have been met
    bool checkGameEnd();
    // calculates and returns final bonus points for a player
    int endGame();
    int getCompleteRows();
    // Checks whether a mosaic position is occupied by a player's tiles
    // Input is required to be within mosaic bounds
    bool occupiedMosaicPosition(int i, int j);
    // Moves a row selected from a factory straight to the broken tiles
    void moveTilesToBrokenTiles(Row row, Lid* lid);
    
  private:
    // Holds all 'broken' tiles (the floor line)
    Row broken_tiles;
    // Player's mosaic
    Mosaic mosaic;
    // Player's storage rows
    Rows rows;
    // Checks whether the row at position i is full of tiles
    // Input is required to be within bounds of the Mosaic
    bool checkFullMosaicRow(int i);
    // Gets the position in the mosaic the colour tile is to be placed
    int getTilePositionForMosaic(Colour colour, int i);
    // Calculates the colour needed for the unfilled board spaces in the mosaic
    Colour populateInitialMosaicPosition(int i);
    // Calculates points awarded at end of round
    int calculateEndRoundPoints(int i, int j);
    // returns the points from the broken tiles at end of round
    int calculateBrokenTilesPoints(Lid* lid);
    // returns the points calculated from complete lines in the mosaic at game 
    // end
    int calculateCompleteLine(int lineNumber);
    // checks whether a mosaic is full of each colour and returns
    // the points gained from it
    int mosaicColourCount();
    //int array to hold the values of broken tiles at each position
    int brokenTilePoints[7] = {1, 1, 2, 2, 2, 3, 3};
    // integer to hold the amount of completed rows on the board
    int completeRows;
};

#endif //BOARD_H
