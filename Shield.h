#include "Enums.h"

#ifndef SHIELD_H
#define SHIELD_H

struct Shield
{
  struct Position *Position;
  char SymbolOne;
  char SymbolTwo;
  char SymbolThree;
  char SymbolFour;
  int Health;
  Symbol SymbolSwitch;
};

#endif
