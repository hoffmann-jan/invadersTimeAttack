#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player
{
  struct Position *Position;
  char Symbol;
  int Health;
  int Score;
} Player;

#endif
