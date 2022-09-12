#ifndef ASSIGN2_LINKEDLIST_H
#define ASSIGN2_LINKEDLIST_H

#include "Tile.h"
#include <string>

using std::string;

class LinkedList
{
public:
   LinkedList();
   ~LinkedList();

   int getSize();
   Tile *getHead();
   Tile *getTail();

   // add new tile to front of list
   void addToFront(Tile *front);
   // add new tile to back of list
   void addToBack(Tile *back);
   // add new tile to position 'i' place in list
   void addToSpecificLocation(Tile *tile, int i);
   // add new tile to random position in list
   void addToRandomLocation(Tile *tile);
   // delete element at location 'i' from list
   void deleteElement(int i);
   // return contents of tile from tile 'i'
   Tile *getTile(int i);
   // extract tile and transfer it to back of 'list'
   void getNewTile(LinkedList *list);
   // extract matching tile and transfer it to random position on 'list', returns false if no match
   bool discardTile(Colour colour, Shape shape, LinkedList *list);
   // prints contents of tile bag
   string printTiles();

private:
   // pointer to the first tile in the linked list
   Tile *head;
   // pointer to the last tile in the linked list
   Tile *tail;
   // size of list
   int size;
};

#endif // ASSIGN2_LINKEDLIST_H
