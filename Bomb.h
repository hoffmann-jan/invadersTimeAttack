#include "stdbool.h"
#include "Enums.h"

#ifndef BOMB_H
#define BOMB_H

typedef struct Bomb
{
  struct Position *Position;
  char Symbol;
  Direction Direction;
  bool Collision;
} Bomb;

#endif
