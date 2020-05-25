#include "Tile.h"

Tile::Tile() 
{
}

Tile::Tile(Colour colour){
    this->colour= colour;
}

Tile::Tile(Colour colour, Tile* next) 
{
   this->colour = colour;
   this->next = next;
}

Tile::Tile(Tile* other){
    this->colour = other->colour;
    this->next = other->next;
}

Tile* Tile::getNext(){
    return next;
}

Colour Tile::getColour(){
    return colour;
}

void Tile::setNext(Tile* next){
    this->next = next;
}

void Tile::setColour(Colour colour){
    this->colour = colour;
}

char Tile::printColour() {
    char colourChar = ' ';
    if(colour == Red) {
        colourChar = RED;
    } else if(colour == Yellow) {
        colourChar = YELLOW;
    } else if(colour == Dark_Blue) {
        colourChar = DARK_BLUE;
    } else if(colour == Light_Blue) {
        colourChar = LIGHT_BLUE;
    } else if(colour == Black) {
        colourChar = BLACK;
    } else if(colour == first_player) {
        colourChar = FIRST_PLAYER;
    } else if(colour == no_tile) {
        colourChar = NO_TILE;
    }else if(colour == board_Red) {
        colourChar = BOARD_RED;
    } else if(colour == board_Yellow) {
        colourChar = BOARD_YELLOW;
    } else if(colour == board_Dark_Blue) {
        colourChar = BOARD_DARK_BLUE;
    } else if(colour == board_Light_Blue) {
        colourChar = BOARD_LIGHT_BLUE;
    } else if(colour == board_Black) {
        colourChar = BOARD_BLACK;
    }
    return colourChar;
}
