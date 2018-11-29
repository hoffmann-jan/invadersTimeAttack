#include "stdbool.h"
#include "Enums.h"

#ifndef PROJECTILE_H
#define PROJECTILE_H

typedef struct Projectile
{
  struct Position *Position;
  char Symbol;
  Direction Direction;
  bool Collision;
} Projectile;

#endif
