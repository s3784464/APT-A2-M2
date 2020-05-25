#ifndef FACTORY_H
#define FACTORY_H
#include <vector>
#include "Board.h"


#define FACTORY_MAX_TILES 4
class Factory {
  public:
    Factory(int number);
    // Adds a tile to the factory
    void addTile(Tile tile);
    // Adds a tile to the factory based on input colour
    void addTile(Colour colour);
    int getNumber();
    // Returns a Row of all tiles in the factory
    Row getAllTiles();
    // Checks whether a factory contains a certain tile colour
    bool factoryContainsColour(Colour colour);
    //Returns a vector of tiles only of chosen colour
    //also removes the chosen tiles from the factory
    Row getTiles(Colour colour);
    Tile getTileAt(int i);
    // Removes all tiles from a factory
    void removeAllTiles();
    // Prints all tiles in the factory
    void printTiles();
    
  private:
  // Row to hold all tiles in the factory
    Row tiles;
    // the factory number
    int number;
};

#endif //FACTORY_H
