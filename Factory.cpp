#include "Factory.h"

Factory::Factory(int number)
{
    this->number = number;
}


void Factory::addTile(Tile tile)
{
    tiles.push_back(tile);
}

void Factory::addTile(Colour colour)
{
    tiles.push_back(Tile(colour));
}

Row Factory::getAllTiles()
{
    return tiles;
}

bool Factory::factoryContainsColour(Colour colour) {
    bool containsColour = false;
    unsigned int i = 0;
    while(!containsColour && i < tiles.size()) {
        if(tiles.at(i).getColour() == colour) {
            containsColour = true;
        }
        ++i;
    }
    return containsColour;
}


Row Factory::getTiles(Colour colour)
{
    Row chosenTiles;

    Row::iterator it = tiles.begin();
    for( ; it != tiles.end(); )
    {
        if (it->getColour() == colour || it->getColour() == first_player)
        {
            //add tiles to the chosenTiles array
            chosenTiles.push_back(*it);

            //erase the chosen tile from the factory
            tiles.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return chosenTiles;
}

Tile Factory::getTileAt(int i)
{
    return tiles.at(i);
}

int Factory::getNumber()
{
    return this->number;
}

void Factory::removeAllTiles() {
    tiles.clear();
}

void Factory::printTiles()
{
    for (Tile tile : tiles)
    {
        std::cout << tileColour(tile.printColour()) << " ";
    }
}

std::string Factory::tileColour(char colour)
{
    std::string stringColour(1, colour);
    std::string colourTile = "";

    //red
    if (colour == 'R' || colour == 'r')
    {
        colourTile = "\u001b[31m" + stringColour + "\u001b[0m";
    }
    //yellow
    else if (colour == 'Y' || colour == 'y')
    {
        colourTile = "\u001b[33m" + stringColour + "\u001b[0m";
    }
    //blue
    else if (colour == 'B' || colour == 'b')
    {
        colourTile = "\u001b[34m" + stringColour + "\u001b[0m";
    }
    //light blue
    else if (colour == 'L' || colour == 'l')
    {
        colourTile = "\u001b[36m" + stringColour + "\u001b[0m";
    }
    //black
    else if (colour == 'U' || colour == 'u')
    {
        colourTile = "\u001b[30m" + stringColour + "\u001b[0m";
    }
    else{
        colourTile = stringColour + "\u001b[0m";
    }

    return colourTile;
}