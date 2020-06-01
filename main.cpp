#include <iostream>
#include <limits>
#include <fstream>
#include "GameState.h"
#include "Persistence.h"

#define TEST 1
#define REGULAR 0
bool seedEntered = false;
int seed = 0;
int mode;
enum Command
{
    save,
    turn,
    unknown
};

void checkForCommands(int argc, char **argv);
void menu();
void credits();
bool newGame();
bool loadGame();
bool runGame(GameState *gameState);
// Takes user input for a turn and either rejects invalid input or plays the commands specified
bool playTurn(GameState *gameState, int playerTurn, bool* gameExit);
bool saveGame(GameState *gameState, bool* gameExit);
// Checks to see if user input for factory chosen is a valid command
bool validFactoryChoice(GameState *gameState, int factoryChoice, Colour *colour);

// Checks to see if user input for tile colour chosen is a valid command
bool validColourInput(char colourChar, Colour *colour);
// Checks to see if user input for storage row chosen is a valid command
bool validRowChoice(GameState *gameState, int rowChoice, int playerTurn, Colour *colour);
// Prints the results of a round to std::cout
void printRoundResults(GameState* gameState);
// Checks whether the EOF key was entered during user input
bool checkExit();

int main(int argc, char **argv)
{   
    mode = REGULAR;
    checkForCommands(argc, argv);
    menu();
    if(mode != TEST) {
    std::cout <<std::endl << "Goodbye" << std::endl;
    }
    return EXIT_SUCCESS;
}

void menu()
{
    int choice;
    bool gameExit = false;
    if(mode != TEST) {
    std::cout << std::endl
              << "Welcome to Azul!" << std::endl
              << "------------------" << std::endl;
    }
    while (!gameExit)
    {   
        if(checkExit()) {
            gameExit = false;
        } else {
            if(mode != TEST) {
                std::cout << std::endl
                    << "Menu" << std::endl
                    << "-----" << std::endl
                    << " 1. New Game\n"
                    << " 2. Load Game\n"
                    << " 3. Credits (Show student information)\n"
                    << " 4. Quit\n"
                    << std::endl
                    << "Enter your choice and press return: " << std::endl;
                std::cout << "> ";
            }
            std::cin >> choice;
            if(checkExit()) {
                gameExit = true;
            } else {
                if (std::cin.fail()) {
                    // If cin fails (input was not a number), reset the cin fail bit remove everything from the input buffer (or until a new line)
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    // Reset choice to an unused integer for the menu selection
                    choice = -1;
                }
                std::cout << std::endl;

                if (choice == 1)
                {
                    gameExit = newGame();
                }
                else if (choice == 2)
                {
                   gameExit = loadGame();
                }
                else if (choice == 3)
                {
                    //Credits
                    credits();
                }
                else if (choice == 4)
                {
                    gameExit = true;
                    // Quit/Save
                } else {
                    if(mode != TEST) { 
                    std::cout << "Invalid choice. Please select again." << std::endl;
                    }
                }
            }
        }
    }
}

void credits()
{
    std::cout << "Credits: " << std::endl
              << "------------------" << std::endl
              << "Student Name: \t\tStudent Number: \tStudent Email:" << std::endl
              << "Jonathon Mitterbacher \ts3784464 \t\ts3784464@student.rmit.edu.au" << std::endl
              << "Michael Weston \t\ts3541742 \t\ts3541742@student.rmit.edu.au" << std::endl
              << "Sebastian Wisidagama \ts3769969 \t\ts3769969@student.rmit.edu.au" << std::endl
              << std::endl;
}

bool newGame()
{
    bool gameExit = false;
    std::cout << "Starting a New Game" << std::endl;
    
    //Player prompts
    std::vector<std::string> playerNames;
    int i = 0;
    // Resets the std::cin buffer to take player input properly
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while(!gameExit  && i < NUM_PLAYERS)
    {
        bool validName = false;
        while(!gameExit && !validName){
        std::string playerName;
        if(mode != TEST) {
            std::cout << "Enter a name for Player " << (i + 1) << ":" << std::endl << "> ";
            
        }

        std::getline(std::cin,playerName);
        if(checkExit()) {
            gameExit = true;
        } else {
            validName = true;
            unsigned int i = 0;
            while(!gameExit && validName && i < playerNames.size()) {
                if(playerName == playerNames[i]) {
                    validName = false;
                    std::cout << "Player name already taken" << std::endl;
                }
                ++i;
            }
        }
            if(validName) {
                playerNames.push_back(playerName);
            }
        }
    ++i;
    }
    if(!gameExit) {
        GameState *gameState = new GameState();
        gameState->setSeedEntered(seedEntered, seed);
        gameState->initializeNewGame(playerNames);
        //start game
        gameExit = runGame(gameState);
        delete gameState;
    }
    return gameExit;
}

bool loadGame()
{
    bool gameExit = false;
    if(mode != TEST){
    std::cout << "Enter save file name: " << std::endl
              << "> ";
    }
    std::string saveFileName;
    std::cin >> saveFileName;

    //TESTING
    saveFileName = "SaveGames/" + saveFileName;
    
    if(mode == TEST) {
        std::cout << "Testing " << saveFileName << std::endl;
    }
    if(checkExit()) {
        gameExit = true;
    } else {
        std::ifstream file(saveFileName);
        if (file.fail())
        {
            std::cout << "No such file exists, aborting load" << std::endl;
        } else {
            GameState* gameState = new GameState();
            Persistence *persistence = new Persistence(saveFileName);
            bool loadSuccess = persistence->loadGame(gameState);
            delete persistence;
            if(loadSuccess) {
            if(mode == TEST) {
                std::cout << "Load Succeeded" << std::endl;
            } 
            gameExit = runGame(gameState);    
            } 
            delete gameState;
        }
    }
    return gameExit;
}

bool runGame(GameState *gameState)
{
    bool roundFinished = true;
    bool gameEnd = false;
    bool gameExit = false;
    bool testComplete = false;
    if(mode != TEST){
        std::cout << "====== Let's Play! ======" << std::endl;
        
    } else {
        std::cout << "Running Test" << std::endl;
    }
    while (!gameEnd && !gameExit && !testComplete)
    {
        int playerTurn = gameState->getTurn();
        std::string playerName = gameState->getPlayers()[playerTurn]->getName();
        if(mode != TEST) {
            if(roundFinished) {
                std::cout << "====== Start Round ======" << std::endl;
                roundFinished = false;
            }
            std::cout << std::endl << "Turn for Player: " << playerName << std::endl;
            std::cout << "Factories:" << std::endl;
            //Print current factory state for each factory
            for (Factory *f : gameState->getFactories())
            {
                std::cout << f->getNumber() << ": ";
                f->printTiles();
                std::cout << std::endl;
            }

        //Print current board for player
        std::cout << "\nMosaic for " << playerName << ':' << std::endl;
        gameState->getBoards()[playerTurn]->printBoard();

        std::cout << "\nEnter your move " << playerName << std::endl <<"> ";

        }

        bool validTurn = false;
        while (!gameExit && !validTurn && !testComplete)
        {
            Command command = unknown;
            std::string moveChoice;

            std::cin >> moveChoice;
            if (checkExit()) {
                gameExit = true;
            } else {
                if (moveChoice == "turn")
                {
                    command = turn;
                }
                else if (moveChoice == "save")
                {
                    command = save;
                }
                else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid command. Valid commands are: turn, save" << std::endl
                            << "> ";
                }
                if (command == turn)
                {
                    if (playTurn(gameState, playerTurn, &gameExit))
                    {
                        validTurn = true;
                        
                        
                    }
                    else
                    {
                        if(!gameExit) {
                            std::cout << "Turn unsuccessful. Command order is [Factory] [Colour] [Row]" << std::endl << "> ";

                        }

                    }
                }
                else if (command == save)
                {
                    if (saveGame(gameState, &gameExit))
                    {
                        if(mode == TEST) {
                            testComplete = true;
                        } else {
                            std::cout << "Save successful" << std::endl << "> ";
                        }
                    }
                    else
                    {   if(!gameExit) {
                        std::cout << "Save unsuccessful" << std::endl << "> ";
                    }
                        
                    }

                }
            }
        }
        
        if(!gameExit && !testComplete){
            //changes turn to next player, keeping it within NUM_PLAYERS
            playerTurn = (playerTurn + 1) % NUM_PLAYERS;
            gameState->setTurn(playerTurn);
            roundFinished = gameState->roundFinished();
            if(roundFinished){
        
                gameState->endRound();
                if(mode != TEST) {
                    std::cout << "======= End Round =======" << std::endl;
                    printRoundResults(gameState);
                }
                gameEnd = gameState->gameFinished();
            }
            if (gameEnd)
            {
                std::cout << "===== Game Finished =====" << std::endl;
                int winner = gameState->endGame();
                std::cout << "= Game End Bonus Points =" << std::endl;
                for(Player* p : gameState->getPlayers()) {
                    std::cout << p->getName() << ": " << p->getPointsGained() << " points" << std::endl;
                }
                std::cout << "====== Final Score ======" << std::endl;
                for(int i = 0; i != NUM_PLAYERS; ++i) {
                    std::cout << gameState->getPlayers()[i]->getName() << ": "  << gameState->getPlayers()[i]->getPoints() << " points" << std::endl;
                    std::cout << "Rows complete: " << gameState->getBoards()[i]->getCompleteRows() << std::endl;

                }
                if(winner == DRAW) {
                    std::cout << "It's a draw!" << std::endl;
                } else {
                    std::cout << gameState->getPlayers()[winner]->getName() << " wins!" << std::endl;
                }
            }
        } 
    }
    if(mode == TEST) {
        std::cout << "Test Finish" << std::endl;
    }
    return gameExit;
}

bool saveGame(GameState *gameState, bool* gameExit)
{
    bool validSave = true;
    std::string saveFileName;
    std::string filePath = "";
    if(mode != TEST) {
        filePath = "./SaveGames/";
    } else {
        filePath = "./Tests/Results/";
    }
    
    std::cin >> saveFileName;
    if(checkExit()) {
        *gameExit = true;
        validSave = false;
    } else {
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            validSave = false;
        }
        if (validSave)
        {
            std::string file = filePath + saveFileName;
            Persistence *persistence = new Persistence(file);
            validSave = persistence->saveGame(gameState);
            delete persistence;
        }
    }
    return validSave;
}

bool checkExit() {
    bool exit = false;
    if (std::cin.eof()) {
        exit = true;
    }
    return exit;
}

bool playTurn(GameState *gameState, int playerTurn, bool* gameExit)
{   
    int factoryChoice;
    char colourInput;
    int rowChoice;
    bool validTurn = true;
    std::cin >> factoryChoice >> colourInput >> rowChoice;
    if(checkExit()) {
        *gameExit = true;
        validTurn = false;
    } else {
        Colour colourChoice = unrecognised;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            validTurn = false;
        }
        bool validColour = false;
        bool validFactory = false;
        bool validRow = false;
        if (validTurn)
        {
            validColour = validColourInput(colourInput, &colourChoice);
            if (validColour)
            {
                validFactory = validFactoryChoice(gameState, factoryChoice, &colourChoice);
            }
            if (validFactory)
            {
                validRow = validRowChoice(gameState, rowChoice, playerTurn, &colourChoice);
            }
            if (!validFactory || !validColour || !validRow)
            {
                validTurn = false;
            }
        }
        if (validTurn)
        {   
            // If tiles were chosen to go straight into the broken tiles
            if(rowChoice == BROKEN_TILES_POSITION) {
                gameState->getBoards()[playerTurn]->moveTilesToBrokenTiles(gameState->getFactories()[factoryChoice]->getTiles(colourChoice), gameState->getLid());
            } else {
                gameState->getBoards()[playerTurn]->addFactoryTilesToRow(gameState->getFactories()[factoryChoice]->getTiles(colourChoice), rowChoice, gameState->getLid());
            }
            if (factoryChoice != 0)
            {
                // If factory chosen wasn't 0, remove all tiles from the
                // factory and place them in factory 0
                for (Tile t : gameState->getFactories()[factoryChoice]->getAllTiles())
                {
                    gameState->getFactories()[0]->addTile(t);
                }
                gameState->getFactories()[factoryChoice]->removeAllTiles();
            }
            std::cout << "Turn successful" << std::endl;
            if(mode != TEST) {
                gameState->getBoards()[playerTurn]->printBoard();
            }
            
        }
    }

    return validTurn;
}

bool validFactoryChoice(GameState *gameState, int factoryChoice, Colour *colour)
{
    bool validFactory = true;
    if (factoryChoice < 0 || factoryChoice >= NUM_FACTORIES)
    {
        validFactory = false;
    }
    if (validFactory)
    {
        if (!gameState->getFactories().at(factoryChoice)->factoryContainsColour(*colour))
        {
            validFactory = false;
        }
    }
    return validFactory;
}

bool validRowChoice(GameState *gameState, int rowChoice, int playerTurn, Colour *colour)
{

    bool validRow = true;
    bool toBroken = false;
    // Used if moving to broken tiles was selected
    if(rowChoice == BROKEN_TILES_POSITION) {
        toBroken = true;
    } else if (rowChoice <= 0 || rowChoice > NUM_ROWS)
    {
        validRow = false;
    }
    // as rowChoice begins at 1 and vectors at 0, -1 from index
    if (validRow && !toBroken)
    {
        if (!gameState->getBoards()[playerTurn]->validRowChoice(rowChoice, colour))
        {
            validRow = false;
        }
    }
    return validRow;
}

bool validColourInput(char colourChar, Colour *colour)
{
    bool validColour = true;
    if (colourChar == RED || colourChar == BOARD_RED)
    {
        *colour = Red;
    }
    else if (colourChar == YELLOW || colourChar == BOARD_YELLOW)
    {
        *colour = Yellow;
    }
    else if (colourChar == DARK_BLUE || colourChar == BOARD_DARK_BLUE)
    {
        *colour = Dark_Blue;
    }
    else if (colourChar == LIGHT_BLUE || colourChar == BOARD_LIGHT_BLUE)
    {
        *colour = Light_Blue;
    }
    else if (colourChar == BLACK || colourChar == BOARD_BLACK)
    {
        *colour = Black;
    }
    else
    {
        validColour = false;
    }
    return validColour;
}

void printRoundResults(GameState* gameState) {
    //Print current board for players
    for(int i = 0; i != NUM_PLAYERS; ++i) {
        std::cout << "\nMosaic for " << gameState->getPlayers()[i]->getName() << ':' << std::endl;
        gameState->getBoards()[i]->printBoard();
    }
    std::cout << "========= Score =========" << std::endl;
    for(Player* p : gameState->getPlayers()) {
        std::string pointGainSign ="";
        if(!(p->getPointsGained() < 0)) {
            pointGainSign = "+";
        }
        std::cout << p->getName() << ": " << p->getPoints()
                << " points (" << pointGainSign << p->getPointsGained() << ")" 
                << std::endl;
        p->setPointsGained(0);
    }

}
void checkForCommands(int argc, char **argv)
{
    if (argc > 1)
    {
        std::string seedString = argv[1];
        try
        {
            seed = std::stoi(seedString);
            seedEntered = true;
            std::cout << "Seed of " << seed << " was successfully entered" << std::endl;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Seed entered was not a number, discarding seed" << std::endl;
        }
        catch (const std::out_of_range &e)
        {
            std::cout << "Seed entered was not in integer range, discarding seed" << std::endl;
        }
    }
    if(argc >= 3) {
        std::string input = argv[2];
        if(input == "test") {
            mode = TEST;
            std::cout << "Testing has been enabled" << std::endl;
        }
    }
}