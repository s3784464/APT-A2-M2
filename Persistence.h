#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include <string>
#include "GameState.h"

// loadfile length is calculated as PLAYER_NUM * 13 + NUM_FACTORIES + 4
#define LOAD_FILE_LENGTH  36
#define COLOUR_PER_LINE 1

class Persistence {
  public:
  Persistence(std::string fileName);
  bool saveGame(GameState* gameState);
  bool loadGame(GameState* gameState);
  
  private:
  
  std::string fileName;

  // Conversion utilities
  Colour convertCharToColour(char ColourChar);
  bool convertStringToInt(std::string input, int* inputInt);
  

  // Tracks whether load is valid
  bool validLoad;
  // Used to output reason load failed
  std::string invalidLoadMessage;

  // Load validation methods

  // Checks whether the savegame to be loaded is of the correct length
  void validLoadFile();
  // Checks whether a tile is a recognised type of tile from a load
  bool validTile(std::stringstream& loadStream, Colour* colour);
  // Checks whether a tile colour is recognised
  bool validTileColour(Colour colour);
  // Checks whether the correct amount of coloured tiles have been loaded
  bool validTileAmount(Colour colour);
  // Searches the existing players for one with a duplicate name
  bool searchPlayerName(std::vector<Player*> players, std::string name);
  // Checks whether a tile is allowed in the storage rows
  bool validRowTile(Colour colour);
  // Checks whether a tile is is allowed in a factory or floor line
  bool validFactoryOrBrokenTile(Colour colour);
  // Checks whether a tile is allowed in the Mosaic
  bool validMosaicTile(Colour colour);
  // Checks whether a tile is either red, light blue, dark blue, yellow or black
  bool validColourTile(Colour colour);
  // Checks whether a row is a valid selection of tiles according to game rules
  bool validRow(Row row);
   // Checks if mosaic is a valid selection of tiles according to game rules
  bool validMosaic(Board* board);
   // Checks whether a tile is allowed at a position in the mosaic
  bool validMosaicPosition(Board* board, int position);
  // Checks whether a row in the storage rows is a valid row
  // according to game rules (the row does not contain a colour
  // that the corresponding line of the mosaic already has filled)
  bool validRowMosaicLine(Board* board, Row row, int rowNumber);
    // Checks whether to see if a game being loaded is already complete
  bool completedGame(Board* board);
  // Checks whether the amount of coloured tiles after all have been loaded is
  // congruent with game rules (20 of each colour, 100 all up)
  bool finalColourTileCount();


  // Keeps track of total number of colour tiles
  int totalTileCount;
  //Keeps track of amount of each colour
  int tileColourCount[NUM_COLOURS];
};

#endif //PERSISTENCE_H
