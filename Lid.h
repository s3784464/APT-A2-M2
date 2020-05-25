#ifndef LID_H
#define LID_H

#include "Tile.h"
#include "LinkedListTileADT.h"
// Class is used to represent the box lid from the boardgame
class Lid : public LinkedListTileADT {
  public:
  Lid();

  ~Lid();
  // Removes all references in the list WITHOUT deleting them
  // Used when moving all of the tiles into the bag
  void emptyLid();

};

#endif //LID_H
