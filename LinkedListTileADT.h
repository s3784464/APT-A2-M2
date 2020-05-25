#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Tile.h"

class LinkedListTileADT {
  public:
    LinkedListTileADT();
    ~LinkedListTileADT();
    // Returns size of linked list
    int size();
    Tile* get(int i);
    // gets the tail of the linked list
    Tile* getTail();
    // Adds a tile to the front of the linked list
    void addFront(Tile* tile);
    // Adds a tile to the back of the linked list
    void addBack(Tile* tile);
    // Removes a tile from the front of the linked list
    void removeFront();
    // Removes a tile from the back of the linked list
    void removeBack();

    // Setters for child class use
    void setHead(Tile* tile);
    void setTail(Tile* tile);
    void setLength(int length);

    // List attributes are protected 
    private:
    // Head of linked list
    Tile* head;
    // Length of linked list
    int length;
    // Tail of linked list
    Tile* tail;
 

};

#endif //LINKED_LIST_H