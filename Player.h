#include "stdbool.h"
#include "Enums.h"

#ifndef PLAYER_H
#define PLAYER_H
struct Player
{
  struct Position *Position;
  char Symbol;
  int Health;
};

#endif
