#include "LinkedListTileADT.h"

LinkedListTileADT::LinkedListTileADT() {
   head = nullptr;
   tail = nullptr;
   length = 0;
}
LinkedListTileADT::~LinkedListTileADT() {
   Tile* current = head;
   Tile* next;
   if(current != nullptr) {
        while( current->getNext() != nullptr ) {
            next = current->getNext();
            delete current;
            current = next;
        }
        delete current;
   } 
}
int LinkedListTileADT::size() {
   return length;
}

Tile* LinkedListTileADT::get(int index) {
   Tile* current = head;

   if (index >= 0 && index < length) {
      int counter = 0;
      while (counter < index) {
         ++counter;
         current = current->getNext();
      }
   }
   return current;
}
Tile* LinkedListTileADT::getTail() {
   return tail;
}

void LinkedListTileADT::addFront(Tile* tile) {

   if (head == nullptr) {
      head = tile;
      tail = tile;
   } else {
      tile->setNext(head);
      head = tile;
   }
   ++length;
}

void LinkedListTileADT::addBack(Tile* tile) {
   if (tail == nullptr) {
      head = tile;
      tail = tile;
   } else {
      Tile* current = tail;
      current->setNext(tile);
      tail = tile;
      // For safety, sets the next for the tail to nullptr
      tail->setNext(nullptr);
   }
   ++length;
}

void LinkedListTileADT::removeBack() {
   if (head != nullptr) {
      Tile* current = head;
      Tile* secondLast = head;
      while (current->getNext() != nullptr) {
         secondLast = current;
         current = current->getNext();
      }
      if (current == head){
         head = nullptr;
      } else if(secondLast == head && current != head) {
         head->setNext(nullptr);
      }
      --length;
   }
}

void LinkedListTileADT::removeFront() {
   if (head != nullptr){
       head = head->getNext();
       --length;
   }
}

void LinkedListTileADT::setHead(Tile* tile) {
   this->head = tile;
}

void LinkedListTileADT::setTail(Tile* tile) {
   this->tail = tile;
}

void LinkedListTileADT::setLength(int length) {
   this->length = length;
}

