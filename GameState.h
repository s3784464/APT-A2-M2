#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "Tile.h"
#include "Factory.h"
#include "Board.h"
#include "Lid.h"
#include "Bag.h"
#include "Player.h"

//Constant used to represent when there is no winner
#define DRAW  -1
// Constants regarding the number of attributes of the game
#define NUM_PLAYERS     2
#define NUM_FACTORIES   6
#define TOTAL_TILE_PER_COLOUR 20
#define TOTAL_COLOUR_TILES 100

// Class contains all entities required for the boardgame
// Including players, factories, boards, the box lid and tile bag
class GameState {
  public:
    GameState();
    ~GameState();
    // Initialises the entities required for a new game of Azul
    void initializeNewGame(std::vector<std::string> playerNames);
    // Creates player objects from a saved game
    void loadPlayer(std::string playerName);
    // Sets player points from a saved game
    void loadPoints(int index, int points);
    // Sets the player turn from a saved game
    void loadTurn(std::string playerName);
    // Sets the seed of the game
    void setSeed(int seed);
    // Checks whether a seed was entered at initial runtime, and if so sets it
    void setSeedEntered(bool seedEntered, int seed);
    // Sets the turn to the player based on the index input
    void setTurn(int turn);
    int getTurn();
    // Checks whether the conditions for a round to end have been met
    bool roundFinished();
    // Checks whether the conditions for a game to end have been met
    bool gameFinished();
    // Ends a round
    void endRound();
    // Ends the game and returns the index of the winning player
    // returns DRAW if no winner
    int endGame();
    std::vector<Factory*> getFactories();
    std::vector<Player*> getPlayers();
    std::vector<Board*> getBoards();
    std::string getTurnPlayerName();
    int getSeed();
    Bag* getBag();
    Lid* getLid();
  private:
  // Holds all factories required for the game
    std::vector<Factory*> factories;
    // Holds all players required for the game
    std::vector<Player*> players;
    // Holds all boards required for the game
    std::vector<Board*> boards;
    // The Box lid
    Lid* lid;
    // The tile bag
    Bag* bag;
    int turn;
    // The seed of the game
    int seed;
    bool seedEntered;
    // Pseudorandomly generates an order for the bag based on the seed
    // Pseudorandomly Generates its own seed if none was input at runtime
    void shuffleBag();
    // Fills all factories at the beginning of a round
    void populateFactories();
    // Checks whether all tiles in the bag have been exhausted
    bool checkEmptyBag();
    // Fills the bag from box lid
    void refillBag();

};

#endif //GAME_STATE_H