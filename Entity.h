#ifndef ENTITY_H
#define ENTITY_H
struct Entity
{
  struct Position *Position;
  char SymbolOne;
  char SymbolTwo;
  bool SymbolSwitch;
  /* use in shield object */
  int Health;
};

#endif