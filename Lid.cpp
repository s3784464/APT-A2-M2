#include "Lid.h"

#include <iostream>
#include <fstream>

Lid::Lid() 
{
// Constructed in parent class (LinkedListTileADT)
}


Lid::~Lid() {
// Calls parent class destructor (LinkedListTileADT destructor)
}

void Lid::emptyLid() {
    setHead(nullptr);
    setTail(nullptr);
    setLength(0);
}


