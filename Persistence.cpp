#include <fstream>
#include <sstream>
#include "Persistence.h"

Persistence::Persistence(std::string fileName)
{
  this->fileName = fileName;
  for(int i = 0; i != NUM_COLOURS; ++i) {
    tileColourCount[i] = 0;
  }
}

bool Persistence::saveGame(GameState *gameState)
{
  bool success = false;
  std::ofstream saveFile(fileName);

  // Player Names
  for (Player *p : gameState->getPlayers())
  {
    saveFile << p->getName() << std::endl;
  }

  // Player Points
  for (Player *p : gameState->getPlayers())
  {
    saveFile << p->getPoints() << std::endl;
  }
  // Player turn
  saveFile << gameState->getTurnPlayerName() << std::endl;

  // Factories
  for (Factory *f : gameState->getFactories())
  {
    for (Tile tile : f->getAllTiles())
    {
      saveFile << tile.printColour() << " ";
    }
    saveFile << std::endl;
  }
  // Player Mosaics
  for (Board *b : gameState->getBoards())
  {
    for (int i = 0; i != NUM_ROWS; i++)
    {
      for (int j = 0; j != NUM_MOSAIC_COLUMNS; j++)
      {
        saveFile << b->getMosaicPosition(i, j).printColour() << " ";
      }
      saveFile << std::endl;
    }
  }
  // Player Rows
  for (Board *b : gameState->getBoards())
  {
    for (Row r : b->getRows())
    {
      for (unsigned int i = 0; i != r.size(); ++i)
      {
        saveFile << r.at(i).printColour() << " ";
      }
      saveFile << std::endl;
    }
  }
  // Player Broken Tiles
  for (Board *b : gameState->getBoards())
  {
    for (Tile t : b->getBrokenTiles())
    {
      saveFile << t.printColour() << " ";
    }
    saveFile << std::endl;
  }
  
  // Box Lid Tiles
  for (int i = 0; i != gameState->getLid()->size(); i++)
  {
    saveFile << gameState->getLid()->get(i)->printColour() << " ";
  }
  saveFile << std::endl;

  // Bag Tiles
  for (int i = 0; i != gameState->getBag()->size(); i++)
  {
    saveFile << gameState->getBag()->get(i)->printColour() << " ";
  }
  saveFile << std::endl;

  // Seed
  saveFile << gameState->getSeed();
  if (!saveFile.fail())
  {
    success = true;
  }
  saveFile.close();
  return success;
}

bool Persistence::loadGame(GameState *gameState)
{
  // boolean to hold whether any part of the load file is invalid
  validLoad = true;
  invalidLoadMessage = "";
  validLoadFile();
  std::ifstream loadFile(fileName);
  std::string loadLine = "";
  
  // Player Names
  if (validLoad)
  {
    for (int i = 0; i != NUM_PLAYERS; ++i)
    {
      if (validLoad)
      {
        // Checks whether there are duplicate player names in the load
        std::getline(loadFile, loadLine);
        if (searchPlayerName(gameState->getPlayers(), loadLine))
        {
          validLoad = false;
          invalidLoadMessage = "Invalid Load: duplicate player names";
        }
        else
        {
          gameState->loadPlayer(loadLine);
        }
      }
    }
  }
  // Player Points
  if (validLoad)
  {
    for (int i = 0; i != NUM_PLAYERS; ++i)
    {
      if (validLoad)
      {
        std::getline(loadFile, loadLine);
        int points = 0;
        if (convertStringToInt(loadLine, &points))
        {
          // Checks to see if points are < 0, which is not allowed in game rules
          if(points < 0) {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: points were negative";
          } else {
            gameState->loadPoints(i, points);
          }
          
        }
        else
        {
          invalidLoadMessage = "Invalid Load: points were not integers";
        }
      }
    }
  }

 
  // Player turn
  if (validLoad)
  {
    std::getline(loadFile, loadLine);
    // checks to see if turn in the load matches a loaded player name
    if (searchPlayerName(gameState->getPlayers(), loadLine))
    {
      gameState->loadTurn(loadLine);
    }
    else
    {
      validLoad = false;
      invalidLoadMessage = "Invalid Load: Turn was for player that doesn't exist";
    }
  }
  // As tiles are now to be added, initialize variables to track them
  totalTileCount = 0;
  bool firstPlayerTokenFound = false;
  // Factories
  if (validLoad)
  {
    for (Factory *f : gameState->getFactories())
    {
      std::getline(loadFile, loadLine);
      std::stringstream loadStream(loadLine);
      int factoryTileCount = 0;
      bool validInput = true;
      while (validLoad && validInput)
      {
        Colour colour = unrecognised;
        validInput = validTile(loadStream, &colour);
        if (validInput)
        {
          if(!validFactoryOrBrokenTile(colour)) {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: Factory contains incorrect tile";
          } else {
            // Checks for factory 0 special case
            if(f->getNumber() != 0) {
              // Checks whether the first player token is in a factory it 
              // shouldn't be
              if(colour == first_player) {
                validLoad = false;
                 invalidLoadMessage = "Invalid Load: Only Factory 0 can contain the first player token";
              } else {
                // If it's not the first player token, increment the number of 
                // tiles in the factory
                ++factoryTileCount;
              }
            } else if(colour == first_player) {
              if(!firstPlayerTokenFound) {
                firstPlayerTokenFound = true;
              } else {
                validLoad = false;
                invalidLoadMessage = "Invalid Load: Multiple first player tokens found";
              }
            }
            if(validLoad) {
              // if tile is valid, load it into the factory
              f->addTile(colour);
            }
          }
        } 
      }
      if (factoryTileCount > FACTORY_MAX_TILES)
      {
        validLoad = false;
        invalidLoadMessage = "Invalid Load: Invalid number of tiles in Factory";
      }
      loadStream.clear();
    }
  }
 
  if(validLoad) {
    if(gameState->roundFinished()) {
      validLoad = false;
      invalidLoadMessage = "Invalid Load: Round not ongoing";
    }
  }
  // Mosaics
  if (validLoad)
  {

    for (Board *b : gameState->getBoards())
    {
      for (int i = 0; i != NUM_ROWS; i++)
      {
        std::getline(loadFile, loadLine);
        std::stringstream loadStream(loadLine);
        int mosaicRowTileCount = 0;
        bool validInput = true;
        while (validLoad && validInput)
        {
          Colour colour = unrecognised;
          validInput = validTile(loadStream, &colour);
          if(validInput)
          {
            if(!validMosaicTile(colour)) {
              validLoad = false;
              invalidLoadMessage = "Invalid Load: Mosaic contains incorrect tile";
            } else if (mosaicRowTileCount == NUM_MOSAIC_COLUMNS)
            {
              validLoad = false;
              invalidLoadMessage = "Invalid Load: Mosaic has too many tiles";
            }
            else
            {
              // If all checks are valid, load into the mosaic
              b->setMosaicPosition(i, mosaicRowTileCount, new Tile(colour));
              ++mosaicRowTileCount;
            }
          } 
        }
        loadStream.clear();
      }
      if(validLoad) {
        bool mosaicValid = validMosaic(b);
        if(!mosaicValid) {
          validLoad = false;
          invalidLoadMessage = "Invalid Load: Mosaic not correct format";        
        } else if(b->checkGameEnd()) {
          validLoad = false;
          invalidLoadMessage = "Invalid Load: Game Already Complete";    
        }
      }
    }
  }

  //Rows
  if (validLoad)
  {
    for (Board *b : gameState->getBoards())
    {
      for (int i = 0; i != NUM_ROWS; i++)
      {
        std::getline(loadFile, loadLine);
        std::stringstream loadStream(loadLine);
        int rowTileCount = 0;
        Row row;
        bool validInput = true;
        while (validLoad && validInput)
        {
          Colour colour = unrecognised;
          validInput = validTile(loadStream, &colour);
          if (validInput)
          {
            if(!validRowTile(colour)) {
              validLoad = false;
              invalidLoadMessage = "Invalid Load: Row contains incorrect tile";

            } else if (rowTileCount > i)
            {
              validLoad = false;
              invalidLoadMessage = "Invalid Load: Row has too many tiles";
            }
            else
            {
              row.push_back(Tile(colour));
              ++rowTileCount;
            }
          } 
        }
        if (validLoad)
        {
          if(validRow(row)) {
            if(validRowMosaicLine(b, row, i)) {
              b->addRow(row);
            } else {
              validLoad = false;
              invalidLoadMessage = "Invalid Load: Mosaic already has colour at row position";
            }
          } else {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: Row is in incorrect format";
          }
        }
        loadStream.clear();
      }
    }
  }

  // Broken Tiles
  if (validLoad)
  {
    for (Board *b : gameState->getBoards())
    {
      std::getline(loadFile, loadLine);
      std::stringstream loadStream(loadLine);
      int brokenTileCount = 0;
      bool validInput = true;
      Row brokenRow;
      while (validLoad && validInput)
      {
        Colour colour = unrecognised;
        validInput = validTile(loadStream, &colour);
        if (validInput)
        {
          if(!validFactoryOrBrokenTile(colour)) {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: Broken tiles contains incorrect tile";
          } else if (brokenTileCount == MAX_BROKEN_TILES)
          {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: Too many broken tiles";
          } else if(firstPlayerTokenFound && colour == first_player) {
            validLoad = false;
            invalidLoadMessage = "Invalid Load: Multiple first player tokens found";
          } else {
            if(colour == first_player) {
              firstPlayerTokenFound = true;
            }
            b->loadBrokenTile(Tile(colour));
            ++brokenTileCount;
          }
        }
      }
      loadStream.clear();
    }
  }
  // check to see if the first player token was found
  if(validLoad) {
    if(!firstPlayerTokenFound) {
      validLoad = false;
      invalidLoadMessage = "Invalid Load: First player token was not found";
    }

  }
  // Box Lid
  if(validLoad)
  {
    std::getline(loadFile, loadLine);
    std::stringstream loadStream(loadLine);
    bool validInput = true;
    while (validLoad && validInput)
    {
      Colour colour = unrecognised;
      validInput = validTile(loadStream, &colour);
      if (validInput)
      {
        if(validColourTile(colour)) {
          gameState->getLid()->addBack(new Tile(colour));
        } else {
          validLoad = false;
          invalidLoadMessage = "Invalid Load: Lid contains incorrect tile";
        }
      }
    }
    loadStream.clear();
  }


  //  Bag
  if (validLoad)
  {
    std::getline(loadFile, loadLine);
    std::stringstream loadStream(loadLine);
    bool validInput = true;
    while (validLoad && validInput)
    {
      Colour colour = unrecognised;
      validInput = validTile(loadStream, &colour);
      if (validInput)
      {
        if(validColourTile(colour)) {
          gameState->getBag()->addBack(new Tile(colour));
        } else {
          validLoad = false;
          invalidLoadMessage = "Invalid Load: Bag contains incorrect tile";
        }
        
      }
    }
    loadStream.clear();
  }

  // Final check of tile amounts
  if(validLoad && !finalColourTileCount()) {
    validLoad = false;
    invalidLoadMessage = "Invalid Load: Incorrect Number of tiles";
  }

  // Seed
  if (validLoad)
  {
    std::getline(loadFile, loadLine);
    int seed = 0;
    if (convertStringToInt(loadLine, &seed))
    {
      gameState->setSeedEntered(true, seed);
    }
    else
    {
      invalidLoadMessage = "Invalid Load: Seed was not an integer";
    }
  }

  if (validLoad)
  {
    std::cout << "Load Successful" << std::endl;
  }
  else
  {
    std::cout << invalidLoadMessage << std::endl;
  }
  return validLoad;
}

Colour Persistence::convertCharToColour(char colourChar)
{
  Colour colour = unrecognised;
  if (colourChar == RED)
  {
    colour = Red;
  }
  else if (colourChar == YELLOW)
  {
    colour = Yellow;
  }
  else if (colourChar == DARK_BLUE)
  {
    colour = Dark_Blue;
  }
  else if (colourChar == LIGHT_BLUE)
  {
    colour = Light_Blue;
  }
  else if (colourChar == BLACK)
  {
    colour = Black;
  }
  else if (colourChar == FIRST_PLAYER)
  {
    colour = first_player;
  }
  else if (colourChar == BOARD_RED)
  {
    colour = board_Red;
  }
  else if (colourChar == BOARD_YELLOW)
  {
    colour = board_Yellow;
  }
  else if (colourChar == BOARD_DARK_BLUE)
  {
    colour = board_Dark_Blue;
  }
  else if (colourChar == BOARD_LIGHT_BLUE)
  {
    colour = board_Light_Blue;
  }
  else if (colourChar == BOARD_BLACK)
  {
    colour = board_Black;
  }
  else if (colourChar == NO_TILE)
  {
    colour = no_tile;
  }
  return colour;
}

bool Persistence::validColourTile(Colour colour) {
  bool validTile = false;
  if(colour == Red || colour == Yellow || colour == Dark_Blue || colour == Light_Blue || colour == Black) {
    validTile = true;
  }
  return validTile;
}


bool Persistence::validFactoryOrBrokenTile(Colour colour) {
  bool validTile = false;
  if(colour == first_player || validColourTile(colour)) {
    validTile = true;
  }
  return validTile;
}
bool Persistence::validMosaic(Board* board) {
  bool validMosaic = true;
  int i = 0;
  while(validMosaic && i != NUM_ROWS * NUM_MOSAIC_COLUMNS) {
    validMosaic = validMosaicPosition(board, i);
    ++i;
    
  }
  return validMosaic;
}

bool Persistence::validRow(Row row) {
  bool validRow = true;
  unsigned int i = 0;
  while(validRow && i != row.size()) {
    Colour colour = row[i].getColour();
    if(colour != no_tile){
      unsigned int j = 0;
      while(validRow && j != i) {
        if(row[j].getColour() != colour) {
          validRow = false;
        }
        ++j;
      }
    }
      ++i;
    }
  return validRow;
}

bool Persistence::validRowMosaicLine(Board* board, Row row, int rowNumber) {
  bool validRow = true;
  Colour colour = row[0].getColour();
  if(colour != no_tile) {
    int i = 0;
    while(validRow && i != NUM_MOSAIC_COLUMNS) {
      if(colour == board->getMosaicPosition(rowNumber, i).getColour()) {
        validRow = false;
      }
      ++i;
    }
  }
  return validRow;
}

bool Persistence::validMosaicPosition(Board* board, int position) {
  bool validPosition = true;
  // gives the 'x' value of the position in the array
  int row = position / NUM_MOSAIC_COLUMNS;
  // gives the 'y' value of the position in the array
  int column = position % NUM_MOSAIC_COLUMNS;
  // gives the value of the colour that a mosaic position is required to be
  int expectedColour = (position - row) % NUM_MOSAIC_COLUMNS;
  Colour tileColour = board->getMosaicPosition(row,column).getColour();
  if(expectedColour == BOARD_RED_POSITION) {
    if(tileColour != Red && tileColour != board_Red) {
      validPosition = false;
    } 
  } else if(expectedColour == BOARD_YELLOW_POSITION) {
    if(tileColour != Yellow && tileColour != board_Yellow) {
      validPosition = false;
    } 
  } else if(expectedColour == BOARD_DARK_BLUE_POSITION) {
    if(tileColour != Dark_Blue && tileColour != board_Dark_Blue) {
      validPosition = false;
    } 
  } else if(expectedColour == BOARD_LIGHT_BLUE_POSITION) {
    if(tileColour != Light_Blue && tileColour != board_Light_Blue) {
      validPosition = false;
    } 
  } else if(expectedColour == BOARD_BLACK_POSITION) {
    if(tileColour != Black && tileColour != board_Black) {
      validPosition = false;
    } 
  } 
  return validPosition;
}
bool Persistence::validMosaicTile(Colour colour) {
  bool validTile = false;
  if(validColourTile(colour)|| colour == board_Red || colour == board_Yellow || colour == board_Dark_Blue || colour == board_Light_Blue || colour == board_Black) {
    validTile = true;
  }
  return validTile;
}

bool Persistence::validRowTile(Colour colour) {
  bool validTile = false;
  if(colour == no_tile || validColourTile(colour)) {
    validTile = true;
  }
  return validTile;
}

bool Persistence::validTileAmount(Colour colour)
{
  bool validTileAmount = true;

  if (colour == Red)
  {
    ++tileColourCount[COLOUR_COUNT_RED];
    ++totalTileCount;
  }
  else if (colour == Yellow)
  {
    ++tileColourCount[COLOUR_COUNT_YELLOW];
    ++totalTileCount;
  }
  else if (colour == Dark_Blue)
  {
    ++tileColourCount[COLOUR_COUNT_DARK_BLUE];
    ++totalTileCount;
  }
  else if (colour == Light_Blue)
  {
    ++tileColourCount[COLOUR_COUNT_LIGHT_BLUE];
    ++totalTileCount;
  }
  else if (colour == Black)
  {
    ++tileColourCount[COLOUR_COUNT_BLACK];
    ++totalTileCount;
  }
  int i = 0;
  while (validTileAmount && i < NUM_COLOURS)
  {
    if (tileColourCount[i] > TOTAL_TILE_PER_COLOUR)
    {
      invalidLoadMessage = "Invalid Load: Incorrect number of Coloured Tiles";
      validLoad = false;
      validTileAmount = false;
    }
    ++i;
  }

  if (validTileAmount && (totalTileCount > TOTAL_COLOUR_TILES))
  {
    invalidLoadMessage = "Invalid Load: Incorrect Number of tiles";
    validLoad = false;
    validTileAmount = false;
  }
  return validTileAmount;
}

bool Persistence::validTile(std::stringstream &loadStream, Colour *colour)
{
  bool validInput = true;
  bool validTile = false;
  char inputChar = ' ';
  loadStream >> inputChar;
  if (loadStream.eof())
  {
    validInput = false;
  }
  if (validInput && loadStream.fail())
  {
    invalidLoadMessage = "Invalid Load: Unrecognised Tile";
    validInput = false;
    validLoad = false;
  }

  if (validInput)
  {
    *colour = convertCharToColour(inputChar);
    validTile = validTileColour(*colour);
  }
  return validTile;
}

bool Persistence::validTileColour(Colour colour)
{
  bool success = false;
  if (colour == unrecognised)
  {
    validLoad = false;
    invalidLoadMessage = "Invalid Load: Unrecognised Tile";
  }
  else
  {
    success = validTileAmount(colour);
  }

  return success;
}

bool Persistence::finalColourTileCount() {
  bool validAmount = true;
  if(totalTileCount != TOTAL_COLOUR_TILES) {
    validAmount = false;
  }
  int i = 0;
  while(validAmount && i != NUM_COLOURS) {
    if(tileColourCount[i] != TOTAL_TILE_PER_COLOUR) {
      validAmount = false;
    }
    ++i;
  }
  return validAmount;
}

bool Persistence::searchPlayerName(std::vector<Player *> players, std::string name)
{
  bool playerFound = false;
  for (Player *p : players)
  {
    if (p->getName() == name)
    {
      playerFound = true;
    }
  }
  return playerFound;
}

bool Persistence::convertStringToInt(std::string input, int *inputInt)
{
  bool successfulConvert = false;
  try
  {
    *inputInt = std::stoi(input);
    successfulConvert = true;
  }
  catch (const std::invalid_argument &e)
  {
    validLoad = false;
  }
  catch (const std::out_of_range &e)
  {
    validLoad = false;
  }

  return successfulConvert;
}

void Persistence::validLoadFile()
{
  bool correctFileLength = false;
  std::ifstream loadFile(fileName);
  std::string loadLine = "";
  int lineCount = 0;
  bool linesExceeded = false;
  while (!linesExceeded && !loadFile.eof())
  {
    if (lineCount == LOAD_FILE_LENGTH)
    {
      linesExceeded = true;
    }
    else
    {
      std::getline(loadFile, loadLine);
      ++lineCount;
    }
  }
  loadFile.close();
  if (!linesExceeded && lineCount == LOAD_FILE_LENGTH)
  {
    correctFileLength = true;
  }

  if (!correctFileLength)
  {
    validLoad = false;
    invalidLoadMessage = "Invalid Load: Load file incorrect size";
  }
}