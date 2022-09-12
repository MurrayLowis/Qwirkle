#ifndef ASSIGN2_PLAYER_H
#define ASSIGN2_PLAYER_H

#include <string>

#include "Tile.h"
#include "LinkedList.h"

using std::string;
using std::to_string;

class Player
{
public:
    Player(string playerName, bool isHuman);
    ~Player();

    string getName();
    // retrieve player linked list of tiles
    LinkedList *getHand();
    // retrieve player current total score
    int getScore();
    // add points to player score
    void addScore(int score);
    // check for tile in player hand and return it - returns nullptr if no match
    Tile *checkTile(Colour checkColour, Shape checkShape);

    // check whether the player is able to make any moves
    bool moveAvailable;
    // define whether player is human or AI controlled
    bool isHuman;

private:
    // player's name
    string playerName;
    // player's current score
    int playerScore;
    // tiles currently in player's hand
    LinkedList *playerHand;
};

#endif // ASSIGN2_PLAYER_H