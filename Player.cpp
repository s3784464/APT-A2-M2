#include "Player.h"

Player::Player(std::string name){
    this->name = name;
    points = 0;
    pointsGained = 0;
}

void Player::setNumber(int number){
    this->player_number = number;
}

std::string Player::getName() {
    return this->name;
}

void Player::setPoints(int points) {
    this->points = points;
}

void Player::addPoints(int points) {
    this->points += points;
    // As points can't be negative due to game rules, set any negative points 
    // to 0
    if(this->points < 0) {
        this->points = 0;
    }
}

void Player::setPointsGained(int points) {
    this->pointsGained = points;
}

int Player::getPointsGained() {
    return this->pointsGained;
}

int Player::getPoints() {
    return this->points;
}