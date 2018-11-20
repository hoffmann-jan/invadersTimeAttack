/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add standard library for allocation*/
#include <stdlib.h>
/* add bool */
#include <stdbool.h>

/* need kbhit implementation */
#include "myconio.h"

/* define terminal functions */
#define ClearTerminal() printf("\033[H\033[J")
#define GoToTerminalPosition(x,y) printf("\033[%d;%dH", (x), (y))
/* define Invader dimension */
#define InvaderPerRow 11
#define InvaderRowCount 5

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

struct list
{
  struct Entity *Entity;
  struct list *Next;
  struct list *Previous;
};

void ShowSplashScreen();
void Draw();

void Init();
void GetInputCommand();

void MoveInvaders();
void MoveProjectiles();
void MovePlayer();
void RemoveDefeatedEntities();
void DetectCollision();
void Shot();

void FreeAll();

int main (void)
{
  /* fields */
  int _Score = 0;
  int _Health = 3;
  struct winsize _TerminalSize;

  //___DEBUGVARIABLEN___
  //enable step by step loop
  int _DEBUG_MODE = 1;
  int _DEBUG_LAST_PRESSED_BUTTON = 0;

  //___ENDDEBUG___

  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &_TerminalSize);

  /* welcome window */
  ShowSplashScreen();

  /* game initialisation */
  Init();

  /* game loop */
  while (true)
  {

    //PlayerInput and DataUpdate
    if(kbhit())
    {
      _DEBUG_LAST_PRESSED_BUTTON = getchar();
    }

    //redraw
    ClearTerminal();
    printf("loop, last key: %d\n",_DEBUG_LAST_PRESSED_BUTTON);

  }
  /* end game loop */

  return 0;
}

void ShowSplashScreen()
{
  ClearTerminal();
  printf("INVADERS\n");
  printf("INVADERS\n");
  printf("INVADERS\n");
  printf("INVADERS\n");
  printf("INVADERS\n");
  printf("INVADERS\n");
  printf("INVADERS\n");

  printf("\npress enter to start .. \n");
  getchar();

  /* clear screen */
  ClearTerminal();
  /* move cursor to top left */
  GoToTerminalPosition(0, 0);
}


void Init()
{


}
