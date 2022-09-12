#include <iostream>
#include <string>
#include "LinkedList.h"
#include "Player.h"
#include "Game.h"

#define MAX_PLAYERS 4
#define MAX_ROWS 26
#define MAX_COLS 26
#define MIN_ROWS 4
#define MIN_COLS 4

using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::string;
using std::vector;

void menu();
string loadGame();
void optionsMenu();
void showCredits();
bool validateInt(string input);
void clearScreen();

Game *game;
bool quit;

// game parameters
char rows;
int cols;
int humanPlayers;
int computerPlayers;
bool options;

// temp variables for user inputs
string inputString;
bool validInput;

int main(void)
{
   // initialise game default values
   rows = 'Z';
   cols = 26;
   humanPlayers = 2;
   computerPlayers = 0;
   quit = false;
   options = false;
   validInput = false;

   // on launch message
   cout << "Welcome to Qwirkle!" << endl;
   cout << "-------------------" << endl;

   while (!quit && !cin.eof())
   {
      menu();
   }

   // checks for Control-D / EOF
   if (cin.eof())
   {
      cout << endl;
      cout << "Goodbye!" << endl;
      cout << endl;
   }

   return EXIT_SUCCESS;
}

void menu()
{
   cout << "Menu" << endl;
   cout << "-------------" << endl;
   cout << "1. New game" << endl;
   cout << "2. Load game" << endl;
   cout << "3. Options" << endl;
   cout << "4. Credits" << endl;
   cout << "5. Quit" << endl;
   cout << "> ";

   getline(cin, inputString);

   // terminate if user quits
   if (!cin.eof())
   {
      if (validateInt(inputString))
      {
         int input = stoi(inputString);
         switch (input)
         {
         case 1:
            // start game with parameters
            game = new Game();
            game->newGame(rows, cols, humanPlayers, computerPlayers);
            // end and delete game
            if (!cin.eof())
            {
               game->gameLoop();
               delete game;
            }
            break;
         case 2:
            game = new Game();
            if (game->loadFile(loadGame()) == true)
            {
               // start game with loaded parameters
               game->gameLoop();
               delete game;
            }
            else if (cin.eof())
            {
               quit = true;
            }
            else
            {
               cout << "File does not exist";
               cout << endl;
            }
            clearScreen();
            break;
         case 3:
            // go to options menu
            options = true;
            while (options && !cin.eof())
            {
               optionsMenu();
            }
            clearScreen();
            break;
         case 4:
            clearScreen();
            showCredits();
            break;
         case 5:
            clearScreen();
            cout << "Goodbye!" << endl;
            cout << endl;
            quit = true;
            break;
         default:
            cout << "Invalid input. Try again:";
            cout << endl;
         }
      }
   }
}

string loadGame()
{
   cout << "Enter name of save file:" << endl;
   cout << "> ";
   getline(cin, inputString);
   cout << endl;

   return inputString;
}

void optionsMenu()
{
   cout << "Options" << endl;
   cout << "-------------" << endl;
   cout << "1. Select players" << endl;
   cout << "2. Change board size" << endl;
   cout << "3. Back" << endl;
   cout << "> ";

   getline(cin, inputString);

   // TODO - add ctrl+D error handling to all levels
   if (!cin.eof())
   {
      if (validateInt(inputString))
      {
         int input = stoi(inputString);
         switch (input)
         {
         case 1:
            // get human player count
            do
            {
               cout << "How many human players? (1 - " + std::to_string(MAX_PLAYERS) + ")" << endl;
               cout << "> ";

               getline(cin, inputString);
               if (validateInt(inputString))
               {
                  int count = stoi(inputString);
                  if (count >= 1)
                  {
                     if (count <= MAX_PLAYERS)
                     {
                        humanPlayers = count;
                        validInput = true;
                     }
                     else
                     {
                        cout << "Maximum of " + std::to_string(MAX_PLAYERS) + " players" << endl;
                     }
                  }
                  else
                  {
                     cout << "Must have at least 1 human player" << endl;
                  }
               }
               else
               {
                  cout << "Invalid input" << endl;
               }
            } while (!validInput && humanPlayers > 0 && !cin.eof());

            // get computer player count
            if (humanPlayers < MAX_PLAYERS)
            {
               validInput = false;
               int maxAIPlayers = MAX_PLAYERS - humanPlayers;
               do
               {
                  cout << "How many AI players? (up to " + std::to_string(maxAIPlayers) + ")" << endl;
                  cout << "> ";
                  getline(cin, inputString);

                  if (validateInt(inputString))
                  {
                     int count = stoi(inputString);
                     if (count + humanPlayers > 1)
                     {
                        if (count <= maxAIPlayers)
                        {
                           computerPlayers += count;
                           validInput = true;
                        }
                        else
                        {
                        // error message for exceeded bounds
                           cout << "Maximum of " + std::to_string(maxAIPlayers) + " AI players" << endl;
                        }
                     }
                     else
                     {
                        cout << "A single player game must have at least 1 AI opponent" << endl;
                     }
                  }
               } while (!validInput && humanPlayers + computerPlayers < 2 && !cin.eof());
            }
            break;
         case 2:
            // get number of rows
            do
            {
               cout << "How many rows? (" + std::to_string(MIN_ROWS) + " - " + std::to_string(MAX_ROWS) + ")" << endl;
               cout << "> ";

               getline(cin, inputString);
               if (validateInt(inputString))
               {
                  int count = stoi(inputString);
                  if (count >= MIN_ROWS)
                  {
                     if (count <= MAX_ROWS)
                     {
                        rows = 'A' + count - 1;
                        validInput = true;
                     }
                     else
                     {
                        // error message for exceeded bounds
                        cout << "Maximum of " + std::to_string(MAX_ROWS) << endl;
                     }
                  }
                  else
                  {
                        // error message for exceeded bounds
                     cout << "Minimum of " + std::to_string(MIN_ROWS) << endl;
                  }
               }
               else
               {
                  cout << "Invalid input" << endl;
               }
            } while (!validInput && !cin.eof());

            // get number of columns
            do
            {
               cout << "How many columns? (" + std::to_string(MIN_COLS) + " - " + std::to_string(MAX_COLS) + ")" << endl;
               cout << "> ";

               getline(cin, inputString);
               if (validateInt(inputString))
               {
                  int count = stoi(inputString);
                  if (count >= MIN_COLS)
                  {
                     if (count <= MAX_COLS)
                     {
                        cols = count;
                        validInput = true;
                     }
                     else
                     {
                        // error message for exceeded bounds
                        cout << "Maximum of " + std::to_string(MAX_COLS) << endl;
                     }
                  }
                  else
                  {
                        // error message for exceeded bounds
                     cout << "Minimum of " + std::to_string(MIN_COLS) << endl;
                  }
               }
               else
               {
                  cout << "Invalid input" << endl;
               }
            } while (!validInput && !cin.eof());
            break;
         case 3:
            options = false;
            break;
         default:
            cout << "Invalid input. Try again:";
            cout << endl;
         }
      }
   }
   else
   {
      quit = true;
   }
}

void showCredits()
{
   cout << "----------------------------------" << endl;
   cout << "This implementation by:" << endl;
   cout << "----------------------------------" << endl;
   cout << endl;
   cout << "Murray Lowis" << endl;
   cout << "Student ID: s3862651" << endl;
   cout << "Email: s3862651@student.rmit.edu.au" << endl;

   cout << endl;
   cout << "----------------------------------" << endl;
   cout << "Base game also developed by:" << endl;
   cout << "----------------------------------" << endl;
   cout << endl;

   cout << "Jacopo Grabar" << endl;
   cout << "Student ID: s3876518" << endl;
   cout << "Email: s3876518@student.rmit.edu.au" << endl;

   cout << endl;
   cout << "Alasdair Cameron" << endl;
   cout << "Student ID: s3884660" << endl;
   cout << "Email: s3884660@student.rmit.edu.au" << endl;

   cout << endl;
   cout << "Kyle Winfield" << endl;
   cout << "Student ID: s3541867" << endl;
   cout << "Email: s3541867@student.rmit.edu.au" << endl;

   cout << "----------------------------------" << endl;
   cout << endl;
}

// confirms that input was an int
bool validateInt(string input)
{
   bool valid = true;
   for (int i = 0; i < (int)input.length(); i++)
   {
      // does not consider negative numbers
      if (!isdigit(input[i]))
      {
         valid = false;
      }
   }
   if (!valid)
   {
      cout << "Invalid input, please try again." << endl;
   }

   return valid;
}

void clearScreen()
{
   // left this commented out for ease of program review by tutor
   // cout << "\x1B[2J\x1B[H";
}