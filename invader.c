/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add bool */ 
#include <stdbool.h>

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

/* list functions */
struct list *AddElement(struct *list, struct list element);
struct list *RemoveElement(struct *list, struct list element);

/* draw support */
void ShowSplashScreen();
void Draw();

/* helpers */
void Init();
void FreeAll();

/* game logic */
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
  struct winsize _TerminalSize;

  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &_TerminalSize);

  /* welcome window */
  ShowSplashScreen();

  /* game initialisation */
  Init();

  /* game loop */
  while (true)
  {
    printf("loop\n");
    break;
  }
  /* end game loop */
  
  return 0;
}

void ShowSplashScreen()
{
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