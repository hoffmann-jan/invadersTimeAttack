/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add bool */ 
#include <stdbool.h>

struct Position
{
  int Row;
  int Column;
};

struct Entity
{
  struct Position *Position;
  char SymbolOne;
  char SymbolTwo;
  bool SymbolSwitch;
};

void ShowSplashScreen();

void MoveInvaders();
void MoveProjectiles();
void MovePlayer();
void RemoveDefeatedEntities();

void DetectCollision();
void Shot();

int main (void)
{
  /* fields */
  int _Score = 0;
  int _Health = 3;

  /* get terminal size */
  struct winsize _TerminalSize;
  ioctl(0, TIOCGWINSZ, &_TerminalSize);

  printf ("lines %d\n", _TerminalSize.ws_row);
  printf ("columns %d\n", _TerminalSize.ws_col);
  /* end terminal size */

  /* game loop */
  while (true)
  {

  }
  /* end game loop */
  
  return 0;
}
