#ifndef BAG_H
#define BAG_H
#include "Tile.h"
#include "LinkedListTileADT.h"
// Class is used to represent the tile bag from the boardgame
class Bag : public LinkedListTileADT {
  public:
    Bag();
    ~Bag();
    void addLidToBag(Tile* head, int size, Tile* tail);
};

#endif //BAG_H
