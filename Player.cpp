#include <iostream>
#include <string>

#include "Player.h"

Player::Player(string playerName, bool isHuman)
{
    this->playerName = playerName;
    this->isHuman = isHuman;
    this->playerScore = 0;
    this->playerHand = new LinkedList();
    this->moveAvailable = true;
}

Player::~Player()
{
    delete playerHand;
}

string Player::getName()
{
    return playerName;
}

LinkedList *Player::getHand()
{
    return playerHand;
}

int Player::getScore()
{
    return this->playerScore;
}

void Player::addScore(int score)
{
    playerScore += score;
}

Tile *Player::checkTile(Colour checkColour, Shape checkShape)
{
    Tile *returnTile = nullptr;
    Tile *tempTile = this->playerHand->getHead();
    bool found = false;
    do
    {
        // return first matching tile in hand, otherwise return nullptr
        if (tempTile->colour == checkColour &&
            tempTile->shape == checkShape)
        {
            returnTile = tempTile;
            found = true;
        }
        else
        {
            tempTile = tempTile->next;
        }
    } while (tempTile != nullptr && !found);

    return returnTile;
}