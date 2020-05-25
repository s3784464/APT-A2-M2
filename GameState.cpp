#include <random>
#include "GameState.h"
GameState::GameState()
{
    // Initialises all game pieces
    bag = new Bag();
    lid = new Lid();

    for (int i = 0; i != NUM_FACTORIES; ++i)
    {
        factories.push_back(new Factory(i));
    }

    for (int i = 0; i != NUM_PLAYERS; i++)
    {
        boards.push_back(new Board());
    }
}

GameState::~GameState() {
    for(Player* p : players) {
        delete p;
    }

    for(Board* b : boards) {
        delete b;
    }

    for(Factory* f : factories) {
        delete f;
    }
    if(bag != nullptr) {
        delete bag;
    }

    if(lid != nullptr) {
        delete lid;
    }
}

void GameState::initializeNewGame(std::vector<std::string> playerNames)
{
    // Initialize Players
    for (int i = 0; i != NUM_PLAYERS; ++i)
    {
        players.push_back(new Player(playerNames[i]));
    }
    shuffleBag();
    turn = 0;
    populateFactories();
    for (Board *b : boards)
    {
        b->initialiseBoard();
    }
}

void GameState::populateFactories()
{
    // Factory 0 gets the first_player tile as its only tile
    factories[0]->addTile(Tile(first_player));
    // loop initialised to i = 1 as factory 0 has already been dealt with
    for (int i = 1; i != NUM_FACTORIES; ++i)
    {
        for (int j = 0; j < FACTORY_MAX_TILES; ++j)
        {   
            // If the bag is empty, refill from the box lid
            if (checkEmptyBag())
            {
                refillBag();
            }
            // Add the first tile in the bag into the factory and remove it
            // from the bag
            Tile *tileToAdd = bag->get(0);
            factories[i]->addTile(tileToAdd);
            bag->removeFront();
        }
    }
}


std::vector<Player *> GameState::getPlayers()
{
    return players;
}

std::vector<Factory *> GameState::getFactories()
{
    return factories;
}

std::vector<Board *> GameState::getBoards()
{
    return boards;
}

int GameState::getTurn()
{
    return turn;
}

int GameState::getSeed()
{
    return seed;
}
Bag *GameState::getBag()
{
    return bag;
}
Lid *GameState::getLid()
{
    return lid;
}

bool GameState::roundFinished()
{
    bool roundFinished = true;
    int i = 0;
    // Checks whether factories are all empty according to game rules
    while (roundFinished && i < NUM_FACTORIES)
    {
        if (factories[i]->getAllTiles().size() != 0)
        {
            roundFinished = false;
        }
        ++i;
    }
    return roundFinished;
}

bool GameState::gameFinished()
{
    bool gameFinished = false;
    int i = 0;
    while (!gameFinished && i < NUM_PLAYERS)
    {
        gameFinished = boards[i]->checkGameEnd();
        ++i;
    }
    return gameFinished;
}

int GameState::endGame()
{
    int winningPoints = DRAW;
    int winner = DRAW;
    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        players[i]->setPointsGained(boards[i]->endGame());
        players[i]->addPoints(players[i]->getPointsGained());
    }
    
    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        if (players[i]->getPoints() > winningPoints)
        {
            winningPoints = players[i]->getPoints();
            winner = i;
        }
        else if (players[i]->getPoints() == winningPoints)
        { 
            // If a player has the same amount of points as the previous top 
            // scorer, set a draw
            winner = DRAW;
        }
    }
    // If there is no clear winner based on points, check other condition for winning (player with most complete mosaic rows)
    if (winner == DRAW)
    {
        int winningRows = DRAW;
        for (int i = 0; i < NUM_PLAYERS; ++i)
        {
            if (players[i]->getPoints() == winningPoints)
            {
                if (boards[i]->getCompleteRows() > winningRows)
                {
                    winningRows = boards[i]->getCompleteRows();
                    winner = i;
                }
                else if (boards[i]->getCompleteRows() == winningRows)
                {   
                    // If all top scorers have the same amount of rows 
                    // complete, it's a draw
                    winner = DRAW;
                }
            }
        }
    }
    return winner;
}

void GameState::endRound()
{
    // First player token is always accounted for, but for safety if is somehow 
    // 'lost', set the turn to the first player.
    int nextRoundFirst = 0;
    for (unsigned int i = 0; i < NUM_PLAYERS; ++i)
    {
        // Checks to see which player has the first player token
        if (boards[i]->hasFirstPlayerToken())
        {
            nextRoundFirst = i;
        }
    }
    for (unsigned int i = 0; i < NUM_PLAYERS; ++i)
    {
        // calculates the points scored in the round for each player
        players[i]->setPointsGained(boards[i]->endRound(lid));
        players[i]->addPoints(players[i]->getPointsGained());
    }
    // Refills the factories
    populateFactories();
    // Returns the index of the first player for next round
    turn = nextRoundFirst;
}

void GameState::loadPlayer(std::string playerName)
{
    players.push_back(new Player(playerName));
}

void GameState::loadPoints(int index, int points)
{
    players[index]->setPoints(points);
}

void GameState::loadTurn(std::string playerName)
{
    bool playerFound = false;
    int i = 0;
    while (!playerFound && i < NUM_PLAYERS)
    {
        if (players[i]->getName() == playerName)
        {
            playerFound = true;
            turn = i;
        }
        ++i;
    }
}

std::string GameState::getTurnPlayerName()
{
    return players[turn]->getName();
}

void GameState::setTurn(int turn)
{
    this->turn = turn;
}

void GameState::setSeed(int seed)
{
    this->seed = seed;
}

void GameState::setSeedEntered(bool seedEntered, int seed)
{
    this->seedEntered = seedEntered;
    if (seedEntered)
    {
        setSeed(seed);
    }
}

bool GameState::checkEmptyBag()
{
    bool empty = false;
    if (bag->size() == 0)
    {
        empty = true;
    }
    return empty;
}

void GameState::refillBag()
{
    bag->addLidToBag(lid->get(0), lid->size(),lid->getTail());
    lid->emptyLid();
}

void GameState::shuffleBag()
{
    bool shuffleComplete = false;
    int tileColourCount[NUM_COLOURS] = {0};
    int totalTileCount = 0;
    int randomTile = 0;

    // if a seed was entered on game startup, use that seed. Otherwise generate a random seed and store it.
    if (!seedEntered)
    {
        std::random_device device;
        seed = device();
    }
    // construct a Mersenne Twister pseudorandom number generator using the seed
    std::mt19937 engine(seed);
    // Make the pseudorandom generator map to integers between 0 and 4
    std::uniform_int_distribution<int> distribution(0, 4);

    while (!shuffleComplete)
    {
        // Get the next pseudorandom integer
        randomTile = distribution(engine);
        /*
         * Colours are mapped as:
         * 0 = Red
         * 1 = Yellow
         * 2 = Dark Blue
         * 3 = Light Blue
         * 4 = Black
        */

        // checks to see if the colour to be added has already had all of its
        // tiles added, if so move on to the next random number, otherwise
        // adds it
        if (tileColourCount[randomTile] < TOTAL_TILE_PER_COLOUR)
        {
            if (randomTile == COLOUR_COUNT_RED)
            {
                bag->addFront(new Tile(Red));
            }
            else if (randomTile == COLOUR_COUNT_YELLOW)
            {
                bag->addFront(new Tile(Yellow));
            }
            else if (randomTile == COLOUR_COUNT_DARK_BLUE)
            {
                bag->addFront(new Tile(Dark_Blue));
            }
            else if (randomTile == COLOUR_COUNT_LIGHT_BLUE)
            {
                bag->addFront(new Tile(Light_Blue));
            }
            else if (randomTile == COLOUR_COUNT_BLACK)
            {
                bag->addFront(new Tile(Black));
            }
            ++tileColourCount[randomTile];
            ++totalTileCount;
        }
        //Once all tiles have been shuffled, method is done
        if (totalTileCount == TOTAL_COLOUR_TILES)
        {
            shuffleComplete = true;
        }
    }
}