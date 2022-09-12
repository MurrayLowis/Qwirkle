#include "Tile.h"
#include <iostream>

Tile::Tile(Colour colour, Shape shape, Tile *next)
{
   this->colour = colour;
   this->shape = shape;
   this->next = next;
}