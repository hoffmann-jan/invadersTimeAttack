#include "stdbool.h"
#include "Enums.h"

#ifndef INVADER_H
#define INVADER_H
struct Invader
{
  struct Position *Position;
  char SymbolOne;
  char SymbolTwo;
  char SymbolThree;
  char SymbolFour;
  bool Health;
  Symbol SymbolSwitch;
  Direction Direction;
};

#endif
