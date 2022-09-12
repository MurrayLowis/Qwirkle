#ifndef ASSIGN2_GAME_H
#define ASSIGN2_GAME_H

#include <string>
#include <vector>

#include "LinkedList.h"
#include "TileCodes.h"
#include "Player.h"

using std::ifstream;
using std::string;
using std::vector;

class Game
{
public:
   Game();
   ~Game();

   // create a new game
   void newGame(char rows, int cols, int humanPlayers, int computerPlayers);
   // save the current game to a file
   void saveGame(string filename);
   // loads game from file
   bool loadFile(string filename);
   // main loop handling gameplay and player turns
   void gameLoop();
   // all actions a player may perform during their turn
   void playerAction(int playerNumber);
   // controls AI player actions
   void computerAction(int playerNumber);
   // populate the tile bag with a new set of tiles for a new game
   void newTileBag(LinkedList *tileBag);
   // output the current gameboard to terminal
   void printBoard(vector<vector<Tile *>> board);
   // checks if list contains tile that is able to be placed on game board
   bool checkForPlayableTiles(LinkedList *list);
   // request input from one of the players
   void getPrompt(Player *player);
   // verify usernames are valid
   bool validateName(string name);
   // verify int input is valid
   bool validateInt(string input);
   // checks if two strings are equivalent - case insensitive
   bool sameCaseInsensitive(string s1, string s2);

   // check if player move is valid under Qwirkle rules
   bool checkValid(Tile *tile, char row, int col);
   // place a player held tile on the board and get new tile - returns score, or zero if move invalid
   int placeTile(Tile *tile, char row, int col);
   // calculate and return score from tile placed at coordinates
   int calculateScore(char row, int col);

   // methods for reading data from save file:
   // reads in a player hand returns linked list
   void readHand(ifstream &stream, Player *player);
   // reads board type line in and returns 2D vector of board
   vector<vector<Tile *>> readBoard(ifstream &stream);
   // reads in board size, returns in int array
   int *readBSize(ifstream &stream);
   // reads in tile bag data from file
   void readBag(ifstream &stream);

   // clears the current contents of the terminal
   void clearScreen();
   // displays error messages to the player
   void displayErrors();

   // prints all tiles currently in tile bag and player hands
   void test();

private:
   vector<vector<Tile *>> board;
   LinkedList *tileBag;
   LinkedList *boardList;
   Player **players;

   // total number of players
   int numPlayers;
   // number of human players
   int humanPlayers;

   // board dimensions
   int rows;
   int cols;

   // check whether first tile has been placed yet
   bool firstMove;
   // check whether the game is still ongoing
   bool gameOver;
   // check whether player scored a 'Qwirkle'
   bool qwirkleScored;

   // number of current player
   int playerTurn;

   // error checks
   // check if cell on board is available
   bool cellOccupied;
   // check if cell on board contains tile in adjacent cells
   bool notAdjacent;
   // check if tile matches adjacent tiles
   bool tileMismatch;
   // check if tile won't exceed max line length of 6 for 'Qwirkle'
   bool lineTooLong;
   // check if tile is in player hand
   bool tileNotInHand;
   // check if there are any duplicate tiles in the line already
   bool sameTileInLine;
   // check if input column is within limits of board
   bool columnRange;
   // generic error
   bool invalidCommand;
};

#endif // ASSIGN2_GAME_H