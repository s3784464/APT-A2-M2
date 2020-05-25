#include "Bag.h"



Bag::Bag() 
{
// Constructed in parent class (LinkedListTileADT)
}


Bag::~Bag() {
// Calls parent class destructor (LinkedListTileADT destructor)
}

void Bag::addLidToBag(Tile* head, int size, Tile* tail) {
    setLength(size);
    setHead(head);
    setTail(tail);
}
