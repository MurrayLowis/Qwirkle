#include <iostream>
#include <random>
#include "LinkedList.h"
#include <string>

using std::string;
using std::to_string;

LinkedList::LinkedList()
{
   this->head = nullptr;
   this->tail = nullptr;
   this->size = 0;
};

LinkedList::~LinkedList()
{
   // I'm lazy, this just calls deleteElement() repeatedly until the list is empty
   while (this->size > 0)
   {
      deleteElement(0);
   }
};

void LinkedList::deleteElement(int i)
{
   // if at least one tile has been created
   if (this->size > 0)
   {
      Tile *tile = this->head;

      // if deleting head
      if (i == 0)
      {
         // mark next tile as new head
         this->head = tile->next;
         tile = nullptr;
         delete tile;
      }
      // if deleting tail
      else if (i == this->size - 1)
      {
         Tile *secondLast = nullptr;
         while (tile->next != nullptr)
         {
            secondLast = tile;
            tile = tile->next;
         }
         // mark 2nd last tile as new tail
         this->tail = tile;
         tile = nullptr;
         delete tile;
         this->tail = nullptr;
         this->tail = secondLast;
      }
      // if deleting other tile
      else
      {
         // identify tile prior to tile to be deleted
         while (i > 1)
         {
            tile = tile->next;
            i--;
         }
         // mark tile for deletion
         Tile *deletedTile = tile->next;
         // update previous tile's 'next' pointer
         tile->next = tile->next->next;
         delete deletedTile;
      }
      // reduce size of list
      this->size--;
   }
   // if no tiles have been created
   else
   {
      std::cout << "No tiles to delete" << std::endl;
   }

   // unlink head from tail upon reducing list to 1 tile
   if (this->size == 1)
   {
      this->head->next = nullptr;
   }
};

void LinkedList::addToFront(Tile *front)
{
   // set as both head and tail if first tile
   if (this->size == 0)
   {
      this->head = front;
      this->tail = front;
   }
   // otherwise just replace head
   else
   {
      front->next = this->head;
      this->head = front;

      // link head to tail upon adding second tile
      if (this->size == 1)
      {
         this->head->next = this->tail;
      }
   }
   this->size++;
};

void LinkedList::addToBack(Tile *back)
{
   // also set as head if first tile
   if (this->size == 0)
   {
      this->head = back;
      this->tail = back;
   }
   else
   {
      // otherwise replace existing tail
      this->tail->next = back;
      this->tail = back;

      // link head to tail upon adding second tile
      if (this->size == 1)
      {
         this->head->next = this->tail;
      }
   }
   this->size++;
};

void LinkedList::addToSpecificLocation(Tile *tile, int i)
{
   // only allow if the list size is <= i
   if (i <= this->size)
   {
      // set as both head and tail if first tile to be created
      if (this->size == 0)
      {
         this->head = tile;
         this->tail = tile;
      }
      // replace head if i == 0
      else if (i == 0)
      {
         tile->next = this->head;
         this->head = tile;
      }
      else
      {
         // link head to tail upon adding second tile
         if (this->size == 1)
         {
            this->head->next = this->tail;
         }

         // check whether to replace tail node
         bool isTail = false;
         if (i == this->size)
         {
            isTail = true;
         }

         // tile after which the new tile is to be placed
         Tile *previous = this->head;

         // iterate until finding preceding tile
         while (i > 1)
         {
            previous = previous->next;
            i--;
         }
         // point new tile to tile whose position it's taking
         tile->next = previous->next;
         // point previous tile to new tile
         previous->next = tile;

         // replace tail pointer if applicable
         if (isTail)
         {
            this->tail = tile;
            tile->next = nullptr;
         }
      }
      this->size++;
   }
   else
   {
      std::cout << "Out of bounds" << std::endl;
   }
};

void LinkedList::addToRandomLocation(Tile *tile)
{
   // create random numer between '0' and the size of the list
   int min = 0;
   int max = this->size;
   std::random_device engine;
   std::uniform_int_distribution<int> distribution(min, max);

   int value = distribution(engine);

   // add tile to list in position randomly selected above
   addToSpecificLocation(tile, value);
};

Tile *LinkedList::getTile(int i)
{
   Tile *tile = nullptr;
   if (i < size)
   {
      // tile to return
      tile = this->head;

      // iterate until finding tile
      while (i > 0)
      {
         tile = tile->next;
         i--;
      }
   }
   else
   {
      std::cout << " No tiles in list" << std::endl;
   }

   return tile;
}

string LinkedList::printTiles()
{
   string tiles = "";
   Colour colour;
   Shape shape;

   Tile *tile = this->head;
   int i = 0;
   while (i < this->size)
   {
      colour = tile->colour;
      shape = tile->shape;

      tiles += colour;
      tiles.append(to_string(shape));

      tile = tile->next;
      i++;
      // add comma delimiter after tiles
      if (i < size)
      {
         tiles.append(",");
      }
   }

   return tiles;
}

void LinkedList::getNewTile(LinkedList *list)
{
   // tile to extract
   Tile *tile = this->head;
   this->head = this->head->next;

   // create tile as a new tile on new list
   tile->next = nullptr;
   list->addToBack(tile);

   // decrement size of old list
   this->size--;

   // unlink head from tail upon reducing list to 1 tile
   if (this->size == 1)
   {
      this->head->next = nullptr;
   }
}

bool LinkedList::discardTile(Colour colour, Shape shape, LinkedList *list)
{
   // identify first matching tile
   Tile *tile = this->getHead();
   Tile *previous = nullptr;
   // return false if no match found
   bool found = false;
   do
   {
      if (tile->colour == colour &&
          tile->shape == shape)
      {
         found = true;
      }
      else
      {
         previous = tile;
         tile = tile->next;
      }
   } while (tile != nullptr && !found);

   // replace tile if a match is found
   if (found)
   {
      // if tile is head
      if (tile == this->head)
      {
         this->head = this->head->next;
      }
      // if tile is tail
      else if (tile == this->tail)
      {
         this->tail = previous;
      }
      // if tile is neither head nor tail
      else
      {
         previous->next = tile->next;
      }

      // move tile link to prepare for next list
      tile->next = nullptr;
      list->addToRandomLocation(tile);

      // decrement size of old list
      this->size--;

      // unlink head from tail upon reducing list to 1 tile
      if (this->size == 1)
      {
         this->head->next = nullptr;
      }
   }
   return found;
}

int LinkedList::getSize()
{
   return this->size;
}

Tile *LinkedList::getHead()
{
   return this->head;
}

Tile *LinkedList::getTail()
{
   return this->tail;
}