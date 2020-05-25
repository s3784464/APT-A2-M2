#include "Board.h"

Board::Board()
{
    completeRows = 0;
}

Tile Board::getMosaicPosition(int i, int j)
{
    return mosaic[i][j];
}

int Board::getTilePositionForMosaic(Colour colour, int i)
{
    int position = 0;

    for (int j = 0; j < NUM_MOSAIC_COLUMNS; ++j)
    {
        Colour posColour = mosaic[i][j].getColour();
        if ((colour == Red && posColour == board_Red) ||
            (colour == Yellow && posColour == board_Yellow) ||
            (colour == Dark_Blue && posColour == board_Dark_Blue) ||
            (colour == Light_Blue && posColour == board_Light_Blue) ||
            (colour == Black && posColour == board_Black))
        {
            position = j;
        }
    }
    return position;
}

void Board::setMosaicPosition(int i, int j, Tile tile)
{
    mosaic[i][j] = tile;
}

void Board::addRow(Row row)
{
    rows.push_back(row);
}

// rowNumber is NOT the position, but the row. position 0 of vector is row 1
void Board::addFactoryTilesToRow(Row factoryRow, int rowNumber, Lid *lid)
{
    if (factoryRow.at(0).getColour() == first_player)
    {
        addBrokenTile(factoryRow.at(0), lid);
        factoryRow.erase(factoryRow.begin());
    }


    Row tempRow = rows.at(rowNumber - 1); //we want to keep the existing tiles that aren't "no_tile" there
    rows.at(rowNumber - 1).clear();       //clears the row

    //add the old tiles EXCLUDING no_tiles back to the row
    for (Tile tile : tempRow)
    {
        if (tile.getColour() != no_tile)
        {
            rows.at(rowNumber - 1).push_back(tile);
        }
    }

    for (Tile t : factoryRow)
    {
        if (rows.at(rowNumber - 1).size() != (unsigned int)rowNumber)
        {
            rows.at(rowNumber - 1).push_back(factoryRow.front());
        }
        else
        {
            addBrokenTile(t, lid);
        }
    }
    while (rows.at(rowNumber - 1).size() != (unsigned int)rowNumber)
    {
        rows.at(rowNumber - 1).push_back(Tile(no_tile));
    }
    factoryRow.clear();
}

void Board::addBrokenTile(Tile tile, Lid *lid)
{
    if (broken_tiles.size() == MAX_BROKEN_TILES)
    {   
        if(tile.getColour() != first_player) {
        lid->addBack(new Tile(&tile));
        } else {
            // If the first player token is to be added but there is not enough
            // room, move the tile in the last floor slot to the lid and replace
            // it with the first_player tile
            Tile* replace = new Tile(broken_tiles.back());
            // for safety, set the next value to nullptr here to stop issues on lid deletion
            replace->setNext(nullptr);
            broken_tiles.pop_back();
            lid->addBack(replace);
            broken_tiles.push_back(tile);
        }
    } else {
        broken_tiles.push_back(tile);
    }
    
}

// Used to move tiles straight from a factory to the broken tiles
void Board::moveTilesToBrokenTiles(Row row, Lid *lid) {
    for(unsigned int i = 0; i < row.size(); ++i) {
        addBrokenTile(row[i], lid);
    }
    row.clear();
}

bool Board::validRowChoice(int rowNumber, Colour *colour)
{
    bool valid = true;
    unsigned int i = 0;
    int j = 0;
    // Checks to see if the mosaic row is already complete for that colour
    while (valid && j < NUM_MOSAIC_COLUMNS)
    {
        if (mosaic[rowNumber - 1][j].getColour() == *colour)
        {
            valid = false;
        }
        ++j;
    }

    // Check to see if row is already full
    if (valid)
    {
        if (rows.at(rowNumber - 1).at(rowNumber - 1).getColour() != no_tile)
        {
            valid = false;
        }
    }

    // Checks to see if the row is either empty or only contains tiles of the same colour as the input colour
    while (valid && i < rows.at(rowNumber - 1).size())
    {
        Colour rowColour = rows.at(rowNumber - 1).at(i).getColour();

        if (rowColour != *colour && rowColour != no_tile)
        {
            valid = false;
        }
        ++i;
    }

    return valid;
}

void Board::loadBrokenTile(Tile tile)
{
    broken_tiles.push_back(tile);
}

Rows Board::getRows()
{
    return rows;
}

Row Board::getBrokenTiles()
{
    return broken_tiles;
}

bool Board::hasFirstPlayerToken()
{
    bool hasToken = false;
    unsigned int i = 0;
    while (!hasToken && i < broken_tiles.size())
    {
        if (broken_tiles[i].getColour() == first_player)
        {
            hasToken = true;
        }
        ++i;
    }
    return hasToken;
}

int Board::endRound(Lid *lid)
{
    int points = 0;
    for (unsigned int i = 0; i < rows.size(); ++i)
    {
        // If the position at the end of the row is occupied by a tile, the row is complete
        Tile mosaicTile = rows[i].back();
        if (mosaicTile.getColour() != no_tile)
        {
            // Locate the position to place the tile in the mosaic
            int colPos = getTilePositionForMosaic(mosaicTile.getColour(), i);
            // Set the tile position in the mosaic
            setMosaicPosition(i, colPos, mosaicTile.getColour());
            // Calculate points for setting the mosaic
            points += calculateEndRoundPoints(i, colPos);
            for (unsigned int j = 1; j < rows[i].size(); ++j)
            {
                // for all other tiles in the row, move them to the lid
                lid->addBack(new Tile(rows[i].at(j)));
            }
            // empty the row
            rows[i].clear();
            
            // fill the row with empty tile placeholders
            rows[i] = initialiseRow(i);
        }
    }
    points -= calculateBrokenTilesPoints(lid);
    return points;
}

int Board::calculateBrokenTilesPoints(Lid *lid)
{
    int points = 0;
    for (unsigned int i = 0; i < broken_tiles.size(); i++)
    {
        points = points + brokenTilePoints[i];
        if (broken_tiles[i].getColour() != first_player)
        {
            lid->addBack(new Tile(broken_tiles[i]));
        }
    }
    broken_tiles.clear();
    return points;
}
int Board::calculateEndRoundPoints(int i, int j)
{
    // points are initialised at 1 because 1 point at least is always awarded for placing a tile in the mosaic
    int points = 1;
    bool line = true;
    // this bool is required to see when both a row and column are found, requiring an extra point to be added
    bool extraPoint = false;
    int y = i - 1;
    // Calculate Columns First, going above then below
    while (line == true && y >= 0)
    {
        if (occupiedMosaicPosition(y, j))
        {
            ++points;
            extraPoint = true;
        }
        else
        {
            line = false;
        }
        --y;
    }
    line = true;
    y = i + 1;
    while (line == true && y < NUM_ROWS)
    {
        if (occupiedMosaicPosition(y, j))
        {
            ++points;
            extraPoint = true;
        }
        else
        {
            line = false;
        }
        ++y;
    }

    // Calculate Row Points next
    line = true;
    int x = j - 1;
    while (line == true && x >= 0)
    {
        if (occupiedMosaicPosition(i, x))
        {
            if (extraPoint)
            {
                ++points;
                extraPoint = false;
            }
            ++points;
        }
        else
        {
            line = false;
        }
        --x;
    }
    line = true;
    x = j + 1;
    while (line == true && x < NUM_MOSAIC_COLUMNS)
    {
        if (occupiedMosaicPosition(i, x))
        {
            if (extraPoint)
            {
                ++points;
                extraPoint = false;
            }
            ++points;
        }
        else
        {
            line = false;
        }
        ++x;
    }
    return points;
}

bool Board::checkGameEnd()
{
    bool gameEnd = false;
    int i = 0;
    while (!gameEnd && i < NUM_ROWS)
    {
        gameEnd = checkFullMosaicRow(i);
        ++i;
    }
    return gameEnd;
}
int Board::endGame()
{
    int points = 0;
    for (int i = 0; i < NUM_ROWS; ++i)
    {
        points += calculateCompleteLine(i);
    }
    points += mosaicColourCount();
    return points;
}

int Board::calculateCompleteLine(int lineNumber)
{
    int points = 0;
    bool fullRow = true;
    bool fullColumn = true;
    int i = 0;
    while (fullRow && i < NUM_MOSAIC_COLUMNS)
    {
        if (!occupiedMosaicPosition(lineNumber, i))
        {
            fullRow = false;
        }
        
        ++i;
    }
    if (fullRow)
        {
            ++completeRows;
            points += POINTS_FULL_ROW;
        }
    i = 0;
    while (fullColumn && i < NUM_ROWS)
    {
        if (!occupiedMosaicPosition(i, lineNumber))
        {
            fullColumn = false;
        }
        ++i;
    }
    if (fullColumn)
    {
        points += POINTS_FULL_COLUMN;
    }
    return points;
}

int Board::mosaicColourCount()
{
    int points = 0;
    int tileColourCount[NUM_COLOURS] = {0};
    for (int i = 0; i < NUM_ROWS; ++i)
    {
        for (int j = 0; j < NUM_MOSAIC_COLUMNS; ++j)
        {
            if (mosaic[i][j].getColour() == Red)
            {
                ++tileColourCount[COLOUR_COUNT_RED];
            }
            else if (mosaic[i][j].getColour() == Yellow)
            {
                ++tileColourCount[COLOUR_COUNT_RED];
            }
            else if (mosaic[i][j].getColour() == Dark_Blue)
            {
                ++tileColourCount[COLOUR_COUNT_RED];
            }
            else if (mosaic[i][j].getColour() == Light_Blue)
            {
                ++tileColourCount[COLOUR_COUNT_RED];
            }
            else if (mosaic[i][j].getColour() == Black)
            {
                ++tileColourCount[COLOUR_COUNT_RED];
            }
        }
    }

    for (int i = 0; i < NUM_COLOURS; ++i)
    {
        if (tileColourCount[i] == NUM_ROWS)
        {
            points += POINTS_FULL_COLOUR;
        }
    }
    return points;
}

bool Board::checkFullMosaicRow(int i)
{
    bool fullRow = true;
    int j = 0;
    while (fullRow && j < NUM_MOSAIC_COLUMNS)
    {
        fullRow = occupiedMosaicPosition(i, j);
        ++j;
    }
    return fullRow;
}
int Board::getCompleteRows()
{
    return completeRows;
}

void Board::printBoard()
{
    for (int i = 0; i != NUM_ROWS; i++)
    {
        std::cout << i + 1 << ": ";

        // Prints leading white space
        for (unsigned int j = 0; j != NUM_ROWS - rows[i].size(); j++)
        {
            std::cout << " "
                      << " ";
        }
        // Prints tiles in each row
        for (unsigned int k = rows[i].size(); k > 0; k--)
        {
            std::cout << rows.at(i).at(k - 1).printColour() << " ";
        }

        //separate storage from mosaic
        std::cout << "|| ";

        //print mosaic row tiles;
        for (int j = 0; j != NUM_MOSAIC_COLUMNS; j++)
        {
            std::cout << mosaic[i][j].printColour() << " ";
        }

        //go to next row/line
        std::cout << std::endl;
    }

    std::cout << "6: broken:";
    for (Tile tile : broken_tiles)
    {
        std::cout << " " << tile.printColour();
    }
    std::cout << std::endl;
}

// populates an empty mosaic and player rows
void Board::initialiseBoard()
{

    for (int i = 0; i != NUM_ROWS; ++i)
    {
        Row row = initialiseRow(i);
        rows.push_back(row);
    }
    for (int i = 0; i != NUM_MOSAIC_COLUMNS * NUM_ROWS; ++i)
    {
        Tile *toAdd = new Tile(populateInitialMosaicPosition(i));
        mosaic[i / NUM_ROWS][i % NUM_MOSAIC_COLUMNS] = toAdd;
    }
}

Row Board::initialiseRow(int length)
{
    Row row;
    for (int i = 0; i <= length; ++i)
    {
        row.push_back(Tile(no_tile));
    }
    return row;
}

bool Board::occupiedMosaicPosition(int i, int j)
{
    bool occupied = false;
    Colour colour = mosaic[i][j].getColour();
    if (colour == Red || colour == Yellow || colour == Dark_Blue || colour == Light_Blue || colour == Black)
    {
        occupied = true;
    }
    return occupied;
}

Colour Board::populateInitialMosaicPosition(int i)
{
    int row = i / NUM_MOSAIC_COLUMNS;
    Colour tileColour = no_tile;
    int tileNumber = (i - row) % NUM_MOSAIC_COLUMNS;
    if (tileNumber == BOARD_DARK_BLUE_POSITION)
    {
        tileColour = board_Dark_Blue;
    }
    else if (tileNumber == BOARD_YELLOW_POSITION)
    {
        tileColour = board_Yellow;
    }
    else if (tileNumber == BOARD_RED_POSITION)
    {
        tileColour = board_Red;
    }
    else if (tileNumber == BOARD_BLACK_POSITION)
    {
        tileColour = board_Black;
    }
    else if (tileNumber == BOARD_LIGHT_BLUE_POSITION)
    {
        tileColour = board_Light_Blue;
    }
    return tileColour;
}
