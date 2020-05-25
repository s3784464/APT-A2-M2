#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>

class Player {
  public:
    Player(std::string name);
    // Sets the player number
    void setNumber(int number);
    int getPoints();
    void setPoints(int points);
    int getPointsGained();
    void setPointsGained(int points);
    // Used to add points to the already existing points
    void addPoints(int points);
    std::string getName();
  private:
    int player_number;
    std::string name;
    int points;
    // Holds the amount of points gained in a round/game end
    int pointsGained;
};

#endif //PLAYER_H
