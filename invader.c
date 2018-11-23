/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add standard library for allocation*/
#include <stdlib.h>
/* add bool */
#include <stdbool.h>
/* malloc etc. */
#include <stdlib.h>
/* thread sleep */
#include <unistd.h>
/* time */
#include <time.h>

/**/
#include "myconio.h"
#include "List.h"
#include "Entity.h"
#include "Position.h"

/* define system call changes */
// NOCH BUGGY!
//#define AvoidReturn() system ("/bin/stty raw")
//#define ResetAvoidReturn() system("/bin/stty cooked")

/* define terminal functions */
#define ClearTerminal() printf("\033[H\033[J")
#define GoToTerminalPosition(row, col) printf("\033[%d;%dH", (row), (col))
#define HideCursor() printf("\e[?25l") 
#define ShowCursor() printf("\e[?25h")
#define ANSI_Color_Reset "\x1b[0m"
#define ANSI_Color_Black "\x1b[30m"
/* define Invader dimension */
#define __InvaderPerRow 11
#define __InvaderRowCount 5
/* define inital values */
#define __InitialPlayerHealth 3
#define __InvaderHorizontalSpace 5
#define __InvaderVerticalSpace 2
/* first koordinates in terminal 1, 1 */
#define __InvaderFirstColumn 1
#define __InvaderFirstRow 1
#define __InvaderAppearence 'V'
#define __InvaderAppearenceTwo 'W'
#define __ShieldObjectHealth 5
#define __ShieldObjectAppearence '#'
#define __ShieldObjectAppearenceLowHealth '~'
#define __PlayerAppearence 'M'
#define __ProjectileAppearence '|'
#define __SpacerBar '_'
/* moving support */
#define __MoveHorizontalStep 2
#define __FramesPerSecond 30

/* debugvariablen */
//enable step by step loop
int _DEBUG_MODE = 1;
int _PRESSED_BUTTON = 0;
int _DEBUG_FRAMECOUNTER = 0;

/* draw support */
void ShowSplashScreen();
void Draw();

/* helpers */
void Init();

void FreeAll();
void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount);
void BuildShields();
void PrintChar(struct List *object);
void DeleteChar(struct Position * position);
void DrawPlayer();


/* game logic */
void MoveInvaders();
void MoveProjectiles();
void MovePlayer(int direction);
void RemoveDefeatedEntities();
void DetectCollision();
void Shoot();
void ValidateInvaderDirection();

/* global variables */
bool invaderDirection = false;
bool invaderMoveForwart = false;
int score = 0;
int bounceCounter = 1;
int playerHealth = 0;
struct winsize terminalSize;
struct List *invaders = NULL;
struct List *shieldObjects = NULL;
struct List *projectiles = NULL;
struct Position *playerPosition = NULL;

int main (void)
{
  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &terminalSize);

  /* welcome window */
  ShowSplashScreen();
  HideCursor();

  /* game initialisation */
  Init();
  
  time_t begin;
  begin = time(NULL);
  
  ClearTerminal();
  Draw();
  printf(ANSI_Color_Black);

  /* game loop */
  while (true)
  {
    /* 1000000 = 1s */
    usleep(1000000 / __FramesPerSecond);

    _DEBUG_FRAMECOUNTER++;
    //PlayerInput and DataUpdate
    if(kbhit())
    {
      printf(ANSI_Color_Reset);
      _PRESSED_BUTTON = getchar();
      MovePlayer(_PRESSED_BUTTON);
      if (_PRESSED_BUTTON == 32)
        Shoot();
      printf(ANSI_Color_Black);
    }

    /* redraw */
    //ClearTerminal();
    //Draw();
    //printf("%d - %ld - Frames:%d", _PRESSED_BUTTON, begin - time(NULL), _DEBUG_FRAMECOUNTER);
    if (bounceCounter % 4 == 0)
    {
      invaderMoveForwart = true;
      bounceCounter = 1;
    }
    
    if (_DEBUG_FRAMECOUNTER % __FramesPerSecond == 0)
    {
      printf(ANSI_Color_Reset);
      MoveInvaders();
      printf(ANSI_Color_Black);
    }

    if (_DEBUG_FRAMECOUNTER % (__FramesPerSecond / 3) == 0)
    {
      printf(ANSI_Color_Reset);
      MoveProjectiles();
      printf(ANSI_Color_Black);
    }
    
    if (_PRESSED_BUTTON == 10) /* ENTER TO EXIT DEBUG */
    {
      ClearTerminal();
      break;
    }
  }
  /* end game loop */
  
  printf(ANSI_Color_Reset);
  ShowCursor();
  return 0;
}

void ShowSplashScreen()
{
  ClearTerminal();
  int fifth = (int) terminalSize.ws_row / 5;
  int startRow = fifth * 2;
  int startColumn = ((int) terminalSize.ws_col / 2) - 25;
  GoToTerminalPosition(startRow, startColumn);
  printf("    _____   ___    _____    ____  __________  _____");
  GoToTerminalPosition(startRow + 1, startColumn);
  printf("   /  _/ | / / |  / /   |  / __ \\/ ____/ __ \\/ ___/");
  GoToTerminalPosition(startRow + 2, startColumn);
  printf("   / //  |/ /| | / / /| | / / / / __/ / /_/ /\\__ \\ ");
  GoToTerminalPosition(startRow + 3, startColumn);
  printf(" _/ // /|  / | |/ / ___ |/ /_/ / /___/ _, _/___/ / ");
  GoToTerminalPosition(startRow + 4, startColumn);
  printf("/___/_/ |_/  |___/_/  |_/_____/_____/_/ |_|/____/  ");

  GoToTerminalPosition(startRow * 4, startColumn + 10);
  printf("press any key to start .. ");
  getch();

  /* clear screen */
  ClearTerminal();
  /* move cursor to top left */
  GoToTerminalPosition(1, 1);
}


void Init()
{
  /* create invaders */
  while ( GetListCount(invaders) < (__InvaderPerRow * __InvaderRowCount) )
  {
    /* allocate invader */
    struct List * listElement = AllocFullListElement();

    /* set properties */
    if (invaders == NULL)
    {
      GetNextPosition(NULL, listElement->Entity->Position, GetListCount(invaders));
    }
    else
    {
      GetNextPosition(GetLastElement(invaders)->Entity->Position, listElement->Entity->Position, GetListCount(invaders));
    }

    listElement->Entity->SymbolOne = __InvaderAppearence;
    listElement->Entity->SymbolTwo = __InvaderAppearenceTwo;
    listElement->Entity->SymbolSwitch = false;

    /* add to List */
    invaders = AddElement(invaders, listElement);
  }

  /* place shield objects */
  BuildShields();

  /* initialise player and score */
  playerHealth = __InitialPlayerHealth;
  score = 0;
  if (playerPosition == NULL)
    playerPosition = (struct Position*) malloc(sizeof(struct Position));

  playerPosition->Column = (int) terminalSize.ws_col / 2;
  playerPosition->Row = ((int) (terminalSize.ws_row / 20)) * 20;
}

void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount)
{
  /* first invader */
  if (lastPosition == NULL)
  {
    newPosition->Row = __InvaderFirstRow;
    newPosition->Column = __InvaderFirstColumn;
    return;
  }

  /* ohters */
  /* new row */
  if (listCount % __InvaderPerRow == 0)
  {
    newPosition->Row = lastPosition->Row + __InvaderVerticalSpace;
    newPosition->Column = __InvaderFirstColumn;
  }
  /* next column */
  else
  {
    newPosition->Row = lastPosition->Row;
    newPosition->Column = lastPosition->Column + __InvaderHorizontalSpace;
  }
}
void BuildShields()
{
  int ninth = (int) terminalSize.ws_col / 9;
  /* set row for the shields */
  int shieldRow = ((int) terminalSize.ws_row / 20) * 15;

  int cursor = 1;
  while (cursor < terminalSize.ws_col)
  {
    if ( (ninth < cursor && cursor <= ninth * 2)
      || (ninth * 3 < cursor && cursor <= ninth * 4)
      || (ninth * 5 < cursor && cursor <= ninth * 6)
      || (ninth * 7 < cursor && cursor <= ninth * 8))
      {
        struct List *shield = AllocFullListElement();
        shield->Entity->Health = __ShieldObjectHealth;
        shield->Entity->Position->Column = cursor;
        shield->Entity->Position->Row = shieldRow;
        shield->Entity->SymbolOne = __ShieldObjectAppearence;
        shield->Entity->SymbolTwo = __ShieldObjectAppearenceLowHealth;
        shield->Entity->SymbolSwitch = true;

        shieldObjects = AddElement(shieldObjects ,shield);
      }

    cursor++;
  }
}

void Draw()
{
  /* draw invaders */
  struct List *invaderList = GetFirstElement(invaders);
  bool continueOperation = true;

  while (continueOperation)
  {
    if (invaderList == NULL)
      break;

    /* draw */
    PrintChar(invaderList);

    if (invaderList->Next == NULL)
      continueOperation = false;
    else
      invaderList = invaderList->Next;
  }

  /* draw shields */
  struct List *shields = GetFirstElement(shieldObjects);
  continueOperation = true;
  while (continueOperation)
  {
    if (shields == NULL)
      break;

    PrintChar(shields);

    if (shields->Next == NULL)
      continueOperation = false;
    else
      shields = shields->Next;
  }

  /* draw projectiles */
  struct List *projectileList = GetFirstElement(projectiles);
  while (projectileList != NULL)
  {
    PrintChar(projectileList);
    projectileList = projectileList->Next;
  }


  /* draw player */
  DrawPlayer();

  /* draw spacer */
  int i = 1;
  while(i <= terminalSize.ws_col)
  {
    GoToTerminalPosition(terminalSize.ws_row - 2, i);
    printf("%c", __SpacerBar);
    i++;
  }

  /* draw score */
  GoToTerminalPosition(terminalSize.ws_row - 1, 1);
  printf("<score: %d>", score);

  /* draw health */
  GoToTerminalPosition(terminalSize.ws_row - 1, (int) terminalSize.ws_col / 2);
  printf("<health: %d>", playerHealth);
}

void PrintChar(struct List *object)
{
  GoToTerminalPosition(object->Entity->Position->Row, object->Entity->Position->Column);
  if (object->Entity->SymbolSwitch)
    printf("%c", object->Entity->SymbolOne);
  else
    printf("%c", object->Entity->SymbolTwo);
}

void DeleteChar(struct Position * position)
{
  GoToTerminalPosition(position->Row, position->Column);
  printf(" ");
}

void MoveInvaders()
{
  struct List *invaderList = GetFirstElement(invaders);

  /* check direction */
  ValidateInvaderDirection();   
  
  while (invaderList != NULL)
  {
    /* delete od position */
    DeleteChar(invaderList->Entity->Position);

    /* move */
    if (invaderMoveForwart)
    {
      invaderList->Entity->Position->Row++;
      invaderList->Entity->SymbolSwitch = !invaderList->Entity->SymbolSwitch;
    }
    else
    {
      if (!invaderDirection) /* true, left; flase, right */
      {
        invaderList->Entity->Position->Column = invaderList->Entity->Position->Column + __MoveHorizontalStep;
      }
      else
      {
        invaderList->Entity->Position->Column = invaderList->Entity->Position->Column - __MoveHorizontalStep;
      }
    }

    /* draw */
    PrintChar(invaderList);

    /* next */
    invaderList = invaderList->Next;
  }
  invaderMoveForwart = false;
}

void MovePlayer(int direction)
{
  if (direction == 67 || direction == 68)
  {
    DeleteChar(playerPosition);
    switch (direction)
    {
      case 68: /* left */
        playerPosition->Column = playerPosition->Column - 1;
        break;
      case 67: /* right */
        playerPosition->Column = playerPosition->Column + 1;
        break;
    }
    DrawPlayer();
  }  
}

void DrawPlayer()
{
  GoToTerminalPosition(playerPosition->Row, playerPosition->Column);
  printf("%c", __PlayerAppearence);
}

void ValidateInvaderDirection()
{
  struct List *list = GetFirstElement(invaders);

  if (list == NULL)
    return;

  int min = list->Entity->Position->Column;
  int max = list->Entity->Position->Column;

  int i = 1;
  while (list != NULL)
  {
    if (list->Entity->Position->Column < min)
      min = list->Entity->Position->Column;

    if (list->Entity->Position->Column > max)
      max = list->Entity->Position->Column;

    list = list->Next;    
    if (i++ > __InvaderPerRow)
      break;
  }

  if (min == 1 && invaderDirection == true)
  {
    invaderDirection = !invaderDirection;
    bounceCounter++;
  }
  if (max == terminalSize.ws_col && invaderDirection == false)
  {
    invaderDirection = !invaderDirection;
    bounceCounter++;
  }
}

void Shoot()
{
  struct List * projectile = AllocFullListElement();

  projectile->Entity->Position->Column = playerPosition->Column;
  projectile->Entity->Position->Row = playerPosition->Row - 1;
  projectile->Entity->SymbolOne = __ProjectileAppearence;
  projectile->Entity->SymbolTwo = __ProjectileAppearence;
  projectile->Entity->SymbolSwitch = true;
  PrintChar(projectile);

  projectiles = AddElement(projectiles, projectile);
}

void MoveProjectiles()
{
  struct List *projectileList = GetFirstElement(projectiles);
  while (projectileList != NULL)
  {
    DeleteChar(projectileList->Entity->Position);
    if (projectileList->Entity->Position->Row <= 1)
    {
      projectileList = (struct List *)RemoveAndDestroyElement(projectileList);
    }
    else
    {
      projectileList->Entity->Position->Row--;
      PrintChar(projectileList);
      projectileList = projectileList->Next;
    }
  }
}
