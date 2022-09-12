#include <iostream>
#include <string>
#include <cctype>
#include <strings.h>
#include <vector>
#include <fstream>
#include <sstream>

#include "Game.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::stringstream;
using std::vector;

Game::Game()
{
    // initialise tileBag and board list
    this->tileBag = new LinkedList;
    this->boardList = new LinkedList;

    // intialise checks to false
    this->gameOver = false;
    this->qwirkleScored = false;
    this->cellOccupied = false;
    this->notAdjacent = false;
    this->tileMismatch = false;
    this->lineTooLong = false;
    this->sameTileInLine = false;
    this->invalidCommand = false;
    this->tileNotInHand = false;
    this->columnRange = false;
}

Game::~Game()
{
    // delete all tiles from all lists
    delete tileBag;
    delete boardList;
    delete[] players;
};

void Game::gameLoop()
{
    while (this->gameOver == false && !cin.eof())
    {
        // perform player action
        if (this->players[playerTurn]->isHuman)
        {
            // human player
            playerAction(this->playerTurn);
        }
        else
        {
            // AI player
            computerAction(this->playerTurn);
        }

        // select next player
        if (this->playerTurn == this->numPlayers - 1)
        {
            this->playerTurn = 0;
        }
        else
        {
            this->playerTurn++;
        }

        // check if any players are still able to play, otherwise end the game
        bool stillPlaying = false;
        for (int i = 0; i < this->numPlayers; i++)
        {
            if (this->players[i]->moveAvailable)
            {
                stillPlaying = true;
            }
        }
        if (!stillPlaying)
        {
            this->gameOver = true;
        }
        // test();
    }

    // game end results display
    if (this->gameOver == true && !cin.eof())
    {
        printBoard(board);
        cout << "Final scores" << endl;
        for (int i = 0; i < this->numPlayers; i++)
        {
            cout << players[i]->getName() << ": " << players[i]->getScore() << " points" << endl;
        }

        int winningScore = 0;
        string winner;
        for (int i = 0; i < this->numPlayers; i++)
        {
            if (players[i]->getScore() > winningScore)
            {
                winningScore = players[i]->getScore();
                winner = players[i]->getName();
            }
        }

        int numWinners = 0;
        for (int i = 0; i < this->numPlayers; i++)
        {
            if (players[i]->getScore() == winningScore)
            {
                numWinners++;
            }
        }

        cout << endl;
        if (numWinners == 1)
        {
            cout << winner << " wins!" << endl;
        }
        else if (this->numPlayers == 2)
        {
            cout << "You're both losers!" << endl;
        }
        else
        {
            cout << numWinners << " way tie!" << endl;
        }
        cout << endl;
    }
}

void Game::playerAction(int playerNumber)
{
    Player *player = this->players[playerNumber];
    clearScreen();
    // forfeit turn if no tiles in player's hand or the tileBag can be placed
    if (!checkForPlayableTiles(player->getHand()) && !checkForPlayableTiles(this->tileBag))
    {
        player->moveAvailable = false;
    }
    else
    {
        player->moveAvailable = true;
        // general purpose error handling for all player inputs
        bool validPrompt = false;
        while (!validPrompt)
        {
            try
            {
                getPrompt(player);
                validPrompt = true;
            }
            catch (...)
            {
                cout << "Invalid input!" << endl;
            }
        }
    }
}

void Game::computerAction(int playerNumber)
{
    Player *player = this->players[playerNumber];
    LinkedList *list = player->getHand();

    player->moveAvailable = true;
    if (checkForPlayableTiles(list))
    {
        // record which move yields the highest score
        int bestScore = 0;
        Tile *bestTile = list->getHead();
        char bestRow = 'A';
        int bestCol = 0;

        char i = 'A';
        // iterate through board to check each cell
        while (i <= this->rows)
        {
            int j = 0;
            while (j < this->cols)
            {
                // skip check if cell is already occupied
                if (board[i][j] == nullptr)
                {
                    // try every tile in hand
                    Tile *tile = list->getHead();
                    int k = 0;
                    while (k < list->getSize())
                    {
                        // check for match
                        if (checkValid(tile, i, j))
                        {
                            // check if better score
                            int tempScore = calculateScore(i, j);
                            if (tempScore >= bestScore)
                            {
                                // record details of best move
                                bestScore = tempScore;
                                bestTile = tile;
                                bestRow = i;
                                bestCol = j;
                            }
                        }
                        tile = tile->next;
                        k++;
                    }
                }
                j++;
            }
            i++;
        }

        // reset errors from checkValid() so they aren't displayed
        this->cellOccupied = false;
        this->notAdjacent = false;
        this->tileMismatch = false;
        this->lineTooLong = false;
        this->sameTileInLine = false;

        if (bestScore > 0)
        {
            // place tile on board and award score
            player->addScore(placeTile(bestTile, bestRow, bestCol));

            // remove tile from list
            player->getHand()->discardTile(bestTile->colour, bestTile->shape, this->boardList);

            // get replacement tile from tilebag
            if (this->tileBag->getSize() > 0)
            {
                this->tileBag->getNewTile(player->getHand());
            }
        }
    }
    // swap tile if none in hand can be played
    else if (checkForPlayableTiles(this->tileBag))
    {
        // discard head tile
        list->discardTile(list->getHead()->colour, list->getHead()->shape, tileBag);
        // get replacement tile from tilebag
        this->tileBag->getNewTile(player->getHand());
    }
    // forfeit turn if no tiles in AI player's hand or the tileBag can be placed
    else
    {
        player->moveAvailable = false;
    }
}

bool Game::checkForPlayableTiles(LinkedList *list)
{
    // the check ends and returns true as soon as a match is found, returns false otherwise
    bool movePossible = false;

    if (list->getSize() != 0)
    {
        char i = 'A';
        // iterate through board to check each cell
        while (i <= this->rows && !movePossible)
        {
            int j = 0;
            while (j < this->cols && !movePossible)
            {
                // skip check if cell is already occupied
                if (board[i][j] == nullptr)
                {
                    // check every tile in list to see if any can be placed
                    Tile *tile = list->getHead();
                    int k = 0;
                    while (k < list->getSize() && !movePossible)
                    {
                        // terminate function when a match is found
                        if (checkValid(tile, i, j))
                        {
                            movePossible = true;
                        }
                        tile = tile->next;
                        k++;
                    }
                }
                j++;
            }
            i++;
        }

        // reset error checks so they aren't displayed
        this->cellOccupied = false;
        this->notAdjacent = false;
        this->tileMismatch = false;
        this->lineTooLong = false;
        this->sameTileInLine = false;
    }

    return movePossible;
}

void Game::newGame(char rows, int cols, int humanPlayers, int computerPlayers)
{
    // initalise game parameters
    this->playerTurn = 0;
    this->firstMove = true;
    this->rows = rows;
    this->cols = cols;
    this->humanPlayers = humanPlayers;
    this->numPlayers = humanPlayers + computerPlayers;

    // intialise board to specified dimensions
    this->board.resize(this->rows + 2);
    for (char i = 64; i <= this->rows + 1; i++)
    {
        this->board[i] = vector<Tile *>(this->cols);
        for (int j = 0; j < this->cols; j++)
        {
            this->board[i][j] = nullptr;
        }
    }

    // populate linked list for tiles stored in the tile bag
    newTileBag(this->tileBag);

    // initialise players array
    this->players = new Player *[this->numPlayers];

    // temp variables for user inputs
    string inputString;
    bool validInput;

    // create player objects
    for (int i = 0; i != this->numPlayers; ++i)
    {
        // create human players
        if (i < this->humanPlayers)
        {
            validInput = false;
            do
            {
                cout << endl;
                cout << "Enter a name for player " << i + 1 << " (letters and spaces only)" << endl;
                cout << "> ";
                getline(cin, inputString);
                if (validateName(inputString))
                {
                    this->players[i] = new Player(inputString, true);
                    validInput = true;
                }
            } while (!validInput);
        }
        // create AI players
        else
        {
            this->players[i] = new Player("AI " + std::to_string(i - this->humanPlayers + 1), false);
        }
    }

    // transfer 6 tiles from tile bag to each player
    for (int i = 0; i < this->numPlayers; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            tileBag->getNewTile(players[i]->getHand());
        }
    }

    cout << endl;
    cout << "Let's Play!" << endl;
}

bool Game::validateName(string name)
{
    bool valid = false;
    // name must contain at least 1 character and only include letters and spaces
    if (name.length() == 0 ||
        name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != string::npos)
    {
        this->invalidCommand = true;
        displayErrors();
    }
    else
    {
        valid = true;
    }
    return valid;
}

bool Game::validateInt(string input)
{
    bool valid = true;
    for (int i = 0; i < (int)input.length(); i++)
    {
        // does not consider negative numbers as valid
        if (!isdigit(input[i]))
        {
            valid = false;
        }
    }
    if (!valid)
    {
        this->invalidCommand = true;
        displayErrors();
    }

    return valid;
}

void Game::newTileBag(LinkedList *tileBag)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            Colour colour = 'R';
            Shape shape = 0;

            // select tile colour
            switch (i)
            {
            case 0:
                colour = RED;
                break;
            case 1:
                colour = ORANGE;
                break;
            case 2:
                colour = YELLOW;
                break;
            case 3:
                colour = GREEN;
                break;
            case 4:
                colour = BLUE;
                break;
            case 5:
                colour = PURPLE;
                break;
            }

            // select tile shape
            switch (j)
            {
            case 0:
                shape = CIRCLE;
                break;
            case 1:
                shape = STAR_4;
                break;
            case 2:
                shape = DIAMOND;
                break;
            case 3:
                shape = SQUARE;
                break;
            case 4:
                shape = STAR_6;
                break;
            case 5:
                shape = CLOVER;
                break;
            }

            // repeated since there are 2 of each tile
            Tile *tile1 = new Tile(colour, shape, nullptr);
            Tile *tile2 = new Tile(colour, shape, nullptr);
            tileBag->addToRandomLocation(tile1);
            tileBag->addToRandomLocation(tile2);
        }
    }
}

void Game::printBoard(vector<vector<Tile *>> board)
{
    // print formatted columns headers
    cout << endl;
    cout << " ";
    for (int i = 0; i < this->cols; i++)
    {
        if (i <= 10)
        {
            cout << "  " << i;
        }
        else
        {
            cout << " " << i;
        }
    }
    cout << endl;
    for (int i = 0; i < this->cols; i++)
    {
        cout << "---";
    }
    cout << "---";
    cout << endl;

    // print grid
    for (char i = 'A'; i <= this->rows; i++)
    {
        // print row headers
        cout << i << " |";
        for (int j = 0; j < this->cols; j++)
        {
            // print double whitespace if cell is empty, otherwise print the tile
            if (board[i][j] == nullptr)
            {
                cout << "  "
                     << "|";
            }
            else
            {
                cout << board[i][j]->colour << board[i][j]->shape << "|";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void Game::saveGame(string filename)
{
    ofstream savefile;
    savefile.open("Saved/" + filename + ".save");

    // total number of players
    savefile << this->numPlayers << endl;

    // number of human players
    savefile << this->humanPlayers << endl;

    for (int i = 0; i < this->numPlayers; i++)
    {
        // player name
        if (this->players[i]->isHuman)
        {
            savefile << this->players[i]->getName() << endl;
        }
        else
        {
            savefile << endl;
        }
        // player score
        savefile << this->players[i]->getScore() << endl;
        // player tiles
        savefile << this->players[i]->getHand()->printTiles() << endl;
    }

    // board dimensions
    savefile << this->cols << "," << this->rows - 64 << endl;

    // board contents
    string boardCont = "";
    for (char i = 'A'; i <= this->rows; i++)
    {
        for (int j = 0; j < this->cols; j++)
        {
            if (board[i][j] != nullptr)
            {
                // Notation is Tile @ Board location (eg. B4@F9)
                boardCont.push_back(board[i][j]->colour);
                boardCont.append(to_string(board[i][j]->shape));
                boardCont.push_back('@');
                boardCont.push_back(i);
                boardCont.append(to_string(j));
                boardCont.push_back(',');
            }
        }
    }
    // remove last comma in csv
    boardCont.pop_back();
    savefile << boardCont << endl;

    // tile bag contents
    savefile << tileBag->printTiles() << endl;

    // which player's turn it is
    savefile << this->playerTurn << endl;

    savefile.close();
}

bool Game::loadFile(string filename)
{
    bool fileExist = false;
    // user does not need to input the ".save" file extension
    ifstream loadfile("Saved/" + filename + ".save");
    if (!loadfile.fail())
    {
        fileExist = true;
        // string holds data as it is read in
        string temp;

        // number of players
        getline(loadfile, temp);
        this->numPlayers = stoi(temp);
        this->players = new Player *[this->numPlayers];

        // number of human players
        getline(loadfile, temp);
        this->humanPlayers = stoi(temp);

        int compPlayers = 0;
        for (int i = 0; i < this->numPlayers; i++)
        {
            // read player name
            getline(loadfile, temp);
            if (temp.length() != 0)
            {
                this->players[i] = new Player(temp, true);
            }
            // computer players have no name
            else
            {
                compPlayers++;
                this->players[i] = new Player("AI " + std::to_string(compPlayers), false);
            }

            // player score
            getline(loadfile, temp);
            this->players[i]->addScore(stoi(temp));

            // player hand
            readHand(loadfile, this->players[i]);
        }

        // board dimensions
        readBSize(loadfile);

        // Board state
        this->board = readBoard(loadfile);

        // tile bag contents
        readBag(loadfile);

        // current player turn
        getline(loadfile, temp);
        this->playerTurn = stoi(temp);
    }
    loadfile.close();

    return fileExist;
    return true;
}

void Game::readBag(ifstream &stream)
{
    string line, tileString;
    char delimiter = ',';

    getline(stream, line);
    stringstream sstream(line);
    Tile *tile;

    while (getline(sstream, tileString, delimiter))
    {
        tile = new Tile(tileString.at(0), tileString.at(1) - 48, nullptr);
        tileBag->addToBack(tile);
    }
}

void Game::readHand(ifstream &stream, Player *player)
{
    string line, tileString;
    char delimiter = ',';

    getline(stream, line);
    stringstream sstream(line);
    Tile *tile;

    while (getline(sstream, tileString, delimiter))
    {
        tile = new Tile(tileString.at(0), tileString.at(1) - 48, nullptr);
        player->getHand()->addToBack(tile);
    }
}

int *Game::readBSize(ifstream &stream)
{
    int *boardSize = new int[2];
    string line, dim;
    char delimiter = ',';

    getline(stream, line);
    stringstream sstream(line);

    int i = 0;
    while (getline(sstream, dim, delimiter))
    {
        boardSize[i] = stoi(dim);
        i++;
    }

    // initialise board size
    this->rows = 'A' - 1 + boardSize[0];
    this->cols = boardSize[1];

    return boardSize;
}

vector<vector<Tile *>> Game::readBoard(ifstream &stream)
{
    string line, coord;
    vector<vector<Tile *>> board(this->rows + 2);

    // char 65 = 'A', so 64 is just before the first letter
    for (char i = 64; i <= this->rows + 1; i++)
    {
        board[i] = vector<Tile *>(this->cols);
        for (int j = 0; j < this->cols; j++)
        {
            board[i][j] = nullptr;
        }
    }

    char delimiter = ',';
    Tile *tile;

    getline(stream, line);
    stringstream sstream(line);

    if (!line.empty())
    {
        while (getline(sstream, coord, delimiter))
        {
            // the -48 converts from number to letter (e.g. '1' + 48 = 'A') ... I think?
            tile = new Tile(coord.at(0), coord.at(1) - 48, nullptr);

            // if statement here to check for 2 digit int on board coord
            if (coord.length() == 5)
            {
                // place tile on board
                board[coord.at(3)][coord.at(4) - 48] = tile;
            }
            else
            {
                string twoDigit;
                twoDigit.push_back(coord.at(4));
                twoDigit.push_back(coord.at(5));
                board[coord.at(3)][stoi(twoDigit)] = tile;
            }

            // also add tile to board linked list
            this->boardList->addToBack(tile);
        }
    }
    else
    {
        // this 'should' never happen unless save files are tampered with
        this->firstMove = true;
    }

    return board;
}

int Game::placeTile(Tile *tile, char row, int col)
{
    // calculate and return score earned
    int score = 0;
    this->board[row][col] = tile;
    score += calculateScore(row, col);

    return score;
}

bool Game::checkValid(Tile *tile, char row, int col)
{
    // only remains valid if all checks are passed
    bool valid = true;

    // row of cell being tested for tile match
    char tempRow = row;
    // column of cell being tested for tile match
    int tempCol = col;
    // length of line of tiles
    int rowLength = 1;
    int colLength = 1;
    // bool to confirm colours match
    bool colourCheck = true;
    // bool to confirm shapes match
    bool shapeCheck = true;

    // immediately return false if tile is null
    if (tile == nullptr)
    {
        this->tileNotInHand = true;
        valid = false;
    }

    // first move ignores other checks
    if (!firstMove)
    {
        // ensure cell does not already contain a tile
        if (valid &&
            board[row][col] != nullptr)
        {
            this->cellOccupied = true;
            valid = false;
        }

        // ensure cell is adjacent an existing tile
        if (valid &&
            (row == 'A' || board[row - 1][col] == nullptr) &&
            (row == this->rows || board[row + 1][col] == nullptr) &&
            (col == 0 || board[row][col - 1] == nullptr) &&
            (col == this->cols - 1 || board[row][col + 1] == nullptr))
        {
            this->notAdjacent = true;
            valid = false;
        }

        // TODO - lambda function for line checks?
        // check vertical line
        // check tiles above
        while (valid && tempRow > 'A' && board[tempRow - 1][tempCol] != nullptr)
        {
            if (board[tempRow - 1][tempCol]->colour != tile->colour)
            {
                colourCheck = false;
            }
            if (board[tempRow - 1][tempCol]->shape != tile->shape)
            {
                shapeCheck = false;
            }
            if (board[tempRow - 1][tempCol]->colour == tile->colour &&
                board[tempRow - 1][tempCol]->shape == tile->shape)
            {
                this->sameTileInLine = true;
                valid = false;
            }
            if (!colourCheck && !shapeCheck)
            {
                this->tileMismatch = true;
                valid = false;
            }
            if (colLength > 5)
            {
                this->lineTooLong = true;
                valid = false;
            }

            colLength++;
            tempRow--;
        }

        // check tiles below
        tempRow = row;
        while (valid && tempRow < this->rows && board[tempRow + 1][tempCol] != nullptr)
        {
            if (board[tempRow + 1][tempCol]->colour != tile->colour)
            {
                colourCheck = false;
            }
            if (board[tempRow + 1][tempCol]->shape != tile->shape)
            {
                shapeCheck = false;
            }
            if (board[tempRow + 1][tempCol]->colour == tile->colour &&
                board[tempRow + 1][tempCol]->shape == tile->shape)
            {
                this->sameTileInLine = true;
                valid = false;
            }
            if (!colourCheck && !shapeCheck)
            {
                this->tileMismatch = true;
                valid = false;
            }
            if (colLength > 5)
            {
                this->lineTooLong = true;
                valid = false;
            }

            colLength++;
            tempRow++;
        }

        // reset and check horizontal line
        // check tiles to left
        tempRow = row;
        colourCheck = true;
        shapeCheck = true;
        while (valid && tempCol > 0 && board[tempRow][tempCol - 1] != nullptr)
        {
            if (board[tempRow][tempCol - 1]->colour != tile->colour)
            {
                colourCheck = false;
            }
            if (board[tempRow][tempCol - 1]->shape != tile->shape)
            {
                shapeCheck = false;
            }
            if (board[tempRow][tempCol - 1]->colour == tile->colour &&
                board[tempRow][tempCol - 1]->shape == tile->shape)
            {
                this->sameTileInLine = true;
                valid = false;
            }
            if (!colourCheck && !shapeCheck)
            {
                this->tileMismatch = true;
                valid = false;
            }
            if (rowLength > 5)
            {
                this->lineTooLong = true;
                valid = false;
            }

            rowLength++;
            tempCol--;
        }

        // check tiles to right
        tempCol = col;
        while (valid && tempCol < this->cols - 1 && board[tempRow][tempCol + 1] != nullptr)
        {
            if (board[tempRow][tempCol + 1]->colour != tile->colour)
            {
                colourCheck = false;
            }
            if (board[tempRow][tempCol + 1]->shape != tile->shape)
            {
                shapeCheck = false;
            }
            if (board[tempRow][tempCol + 1]->colour == tile->colour &&
                board[tempRow][tempCol + 1]->shape == tile->shape)
            {
                this->sameTileInLine = true;
                valid = false;
            }
            if (!colourCheck && !shapeCheck)
            {
                this->tileMismatch = true;
                valid = false;
            }
            if (rowLength > 5)
            {
                this->lineTooLong = true;
                valid = false;
            }

            rowLength++;
            tempCol++;
        }
    }
    return valid;
};

int Game::calculateScore(char row, int col)
{
    int score = 0;
    if (!this->firstMove)
    {
        // length of line of tiles
        int rowLength = 0;
        int colLength = 0;

        // row of cell being tested
        char tempRow = row;
        // column of cell being tested
        int tempCol = col;

        // vertical line
        // tiles above
        while (board[tempRow - 1][tempCol] != nullptr && tempRow > 'A')
        {
            colLength++;
            tempRow--;
        }
        tempRow = row;

        // tiles below
        while (board[tempRow + 1][tempCol] != nullptr && tempRow < this->rows)
        {
            colLength++;
            tempRow++;
        }
        tempRow = row;

        // horizontal line
        // tiles to left
        while (board[tempRow][tempCol - 1] != nullptr && tempCol > 0)
        {
            rowLength++;
            tempCol--;
        }
        tempCol = col;

        // tiles to right
        while (board[tempRow][tempCol + 1] != nullptr && tempCol < this->cols)
        {
            rowLength++;
            tempCol++;
        }

        // add point for first tile in each line (not counted by above algorithm)
        if (rowLength > 0)
        {
            rowLength++;
        }
        if (colLength > 0)
        {
            colLength++;
        }

        // add 6 bonus points for each 'quirkle' completed
        if (rowLength > 5)
        {
            rowLength += 6;
            this->qwirkleScored = true;
        }
        if (colLength > 5)
        {
            colLength += 6;
            this->qwirkleScored = true;
        }
        score = rowLength + colLength;
    }
    else
    {
        // first move always scores 1 point
        score = 1;
        this->firstMove = false;
    }

    return score;
}

void Game::getPrompt(Player *player)
{
    // display board
    printBoard(board);

    // display message for scoring a Qwirkle
    if (this->qwirkleScored)
    {
        cout << "Qwirkle scored!" << endl;
        this->qwirkleScored = false;
    }

    // display player stats
    for (int i = 0; i < this->numPlayers; i++)
    {
        cout << this->players[i]->getName() << ": " << this->players[i]->getScore() << " points";
        // notify players whose turns were skipped due to having no available moves
        if (!this->players[i]->moveAvailable)
        {
            cout << " - no moves available";
        }
        cout << endl;
    }
    cout << endl;
    cout << player->getName() << ", it's your turn, your hand is:" << endl;
    cout << player->getHand()->printTiles() << endl;
    cout << endl;

    string input;

    // flag variable to check if input is valid:
    bool valid = false;

    // user prompt asked until a valid input is found
    while (valid == false && input != "^D" && !cin.eof())
    {
        cout << "> ";
        getline(cin, input);
        cout << endl;

        // place a tile on board
        // if the first word is 'place', the third word is 'at', and the input is not longer than 15 characters
        if (sameCaseInsensitive(input.substr(0, 5), "place") &&
            input.length() > 13 &&
            sameCaseInsensitive(input.substr(9, 2), "at") &&
            input.length() <= 15)
        {
            Colour colour = toupper(input.at(6));
            Shape shape = input.at(7) - '0';
            char row = toupper(input.at(12));
            int col = 0;

            // TODO - unify this if/else
            if (input.length() == 14)
            {
                col = input.at(13) - '0';
                // check if tile is in player's hand and the move is valid
                Tile *tile = player->checkTile(colour, shape);
                if (checkValid(tile, row, col))
                {
                    // place tile on board and award score
                    player->addScore(placeTile(tile, row, col));

                    // remove tile from list
                    player->getHand()->discardTile(tile->colour, tile->shape, this->boardList);

                    // get replacement tile from tilebag
                    if (this->tileBag->getSize() > 0)
                    {
                        this->tileBag->getNewTile(player->getHand());
                    }

                    valid = true;
                }
                else
                {
                    // display error message and display tiles to player again after a failure
                    displayErrors();
                    cout << endl;
                    cout << "Tiles available:" << endl;
                    cout << player->getHand()->printTiles() << endl;
                }
            }
            else if (input.length() == 15)
            {
                col = stoi(input.substr(13, 2));

                if (col > this->cols - 1)
                {
                    valid = false;

                    // display error message
                    columnRange = true;
                    displayErrors();
                    cout << endl;
                    cout << "Tiles available:" << endl;
                    cout << player->getHand()->printTiles() << endl;
                }
                else
                {
                    // check if tile is in player's hand and the move is valid
                    Tile *tile = player->checkTile(colour, shape);
                    if (checkValid(tile, row, col))
                    {
                        // place tile on board and award score
                        player->addScore(placeTile(tile, row, col));

                        // remove tile from list
                        player->getHand()->discardTile(tile->colour, tile->shape, this->boardList);

                        // get replacement tile from tilebag
                        if (this->tileBag->getSize() > 0)
                        {
                            this->tileBag->getNewTile(player->getHand());
                        }

                        valid = true;
                    }
                    else
                    {
                        // display error message and display tiles to player again after a failure
                        displayErrors();
                        cout << endl;
                        cout << "Tiles available:" << endl;
                        cout << player->getHand()->printTiles() << endl;
                    }
                }
            }
        }
        // swap a tile with one from the bag if the first word is 'replace' and the input is not longer than 10 characters
        else if (sameCaseInsensitive(input.substr(0, 7), "replace") && input.length() == 10)
        {
            Colour colour = toupper(input.at(8));
            Shape shape = input.at(9) - '0';

            // check if tile is in player's hand, if so replace it with another tile from the tilebag
            if (player->getHand()->discardTile(colour, shape, tileBag))
            {
                // get replacement tile from tilebag
                this->tileBag->getNewTile(player->getHand());
                valid = true;
            }
            else
            {
                // display error
                this->tileNotInHand = true;
                displayErrors();
                cout << endl;
                cout << "Tiles available:" << endl;
                cout << player->getHand()->printTiles() << endl;
            }
        }
        // command to save game
        // if the first world is "save" and the file name has at least one letter
        else if (sameCaseInsensitive(input.substr(0, 4), "save") && input.length() >= 5)
        {
            // game cannot be saved until at least one tile has been placed on the board
            if (firstMove == false)
            {
                string filename = input.erase(0, 5);
                saveGame(filename);

                // ask for prompt again as save is an extra action that shouldn't prevent the player to make a move
                cout << "Game successfully saved as '" << filename << "'." << endl;
                cout << "What is your next move?" << endl;
                valid = false;
            }
            else
            {
                cout << "No game data to save" << endl;
            }
        }
        // command to quit game
        else if (input == "^D" || cin.eof())
        {
            // stop game
            this->gameOver = true;

            // stop loop
            valid = true;
        }
        else
        {
            this->invalidCommand = true;
            displayErrors();
            cout << endl;
            cout << "Tiles available:" << endl;
            cout << player->getHand()->printTiles() << endl;
        }
    }
}

bool Game::sameCaseInsensitive(string s1, string s2)
{
    if (strcasecmp(s1.c_str(), s2.c_str()) == 0)
    {
        return true;
    }
    return false;
}

void Game::clearScreen()
{
    // left this commented out for ease of program review by tutor
    // cout << "\x1B[2J\x1B[H";
}

void Game::displayErrors()
{
    // display error messages to player and reset error
    if (this->cellOccupied)
    {
        cout << "Cell already contains a tile, please try again" << endl;
        this->cellOccupied = false;
    }
    if (this->notAdjacent)
    {
        cout << "Tile must be placed next to another tile, please try again" << endl;
        this->notAdjacent = false;
    }
    if (this->tileMismatch)
    {
        cout << "Tile does not match, please try again" << endl;
        this->tileMismatch = false;
    }
    if (this->lineTooLong)
    {
        cout << "Line cannot exceed 6 tiles, please try again" << endl;
        this->lineTooLong = false;
    }
    if (this->tileNotInHand)
    {
        cout << "No such tile in hand, please try again" << endl;
        this->tileNotInHand = false;
    }
    if (this->sameTileInLine)
    {
        cout << "Can't have duplicate tiles in the same line, please try again" << endl;
        this->sameTileInLine = false;
    }
    if (this->columnRange)
    {
        cout << "Column value out of range, please try again" << endl;
        this->columnRange = false;
    }
    // generic error if you don't want to create a new one
    if (this->invalidCommand)
    {
        cout << "Invalid input, please try again" << endl;
        this->invalidCommand = false;
    }
}

void Game::test()
{
    cout << "Tile bag" << endl;
    Tile *tile = this->tileBag->getHead();
    while (tile != nullptr)
    {
        cout << tile->colour << tile->shape << ",";
        tile = tile->next;
    }
    cout << endl;

    for (int i = 0; i < this->numPlayers; i++)
    {
        cout << "Player " << i << endl;
        tile = this->players[i]->getHand()->getHead();
        while (tile != nullptr)
        {
            cout << tile->colour << tile->shape << ",";
            tile = tile->next;
        }
        cout << endl;
    }
    cout << endl;
}