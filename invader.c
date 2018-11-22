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

/* define terminal functions */
#define ClearTerminal() printf("\033[H\033[J")
#define GoToTerminalPosition(row, col) printf("\033[%d;%dH", (row), (col))
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

/* debugvariablen */
//enable step by step loop
int _DEBUG_MODE = 1;
int _DEBUG_LAST_PRESSED_BUTTON = 0;
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


/* game logic */
void MoveInvaders();
void MoveProjectiles();
void MovePlayer();
void RemoveDefeatedEntities();
void DetectCollision();
void Shot();

/* global variables */
int score = 0;
int playerHealth = 0;
struct winsize terminalSize;
struct List *invaders = NULL;
struct List *shieldObjects = NULL;
struct Position *playerPosition = NULL;

int main (void)
{
  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &terminalSize);

  /* welcome window */
  ShowSplashScreen();

  /* game initialisation */
  Init();
  Draw(true);
  
  time_t secondStart;
  secondStart = time(NULL);
  
  

  ClearTerminal();
  Draw();
  usleep(1000);

  /* game loop */
  while (true)
  {
    _DEBUG_FRAMECOUNTER++;
    //PlayerInput and DataUpdate
    if(kbhit())
    {
      _DEBUG_LAST_PRESSED_BUTTON = getchar();
    }

    /* redraw */
    ClearTerminal();
    Draw();

    /* 500 = 0,5s */
    usleep(10000);
    printf("%d", _DEBUG_LAST_PRESSED_BUTTON);
    
    if (_DEBUG_LAST_PRESSED_BUTTON == 10) /* ENTER TO EXIT DEBUG */
    {
      ClearTerminal();
      break;
    }
  }
  /* end game loop */

  return 0;
}

void ShowSplashScreen()
{
  ClearTerminal();
  printf("    _____   ___    _____    ____  __________  _____\n");
  printf("   /  _/ | / / |  / /   |  / __ \\/ ____/ __ \\/ ___/\n");
  printf("   / //  |/ /| | / / /| | / / / / __/ / /_/ /\\__ \\ \n");
  printf(" _/ // /|  / | |/ / ___ |/ /_/ / /___/ _, _/___/ / \n");
  printf("/___/_/ |_/  |___/_/  |_/_____/_____/_/ |_|/____/  \n");
  printf("\n");
  printf("\n");

  printf("\npress enter to start .. \n");
  getchar();

  /* clear screen */
  ClearTerminal();
  /* move cursor to top left */
  GoToTerminalPosition(0, 0);
}


void Init()
{
  /* create invaders */
  while ( GetListCount(invaders) <= (__InvaderPerRow * __InvaderRowCount) )
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
  int shildRow = ((int) terminalSize.ws_row / 20) * 15;

  int cursor = 0;
  while (cursor < terminalSize.ws_col)
  {
    GoToTerminalPosition(cursor, shildRow);

    if ( (ninth < cursor && cursor < ninth * 3)
      || (ninth * 3 < cursor && cursor < ninth * 5)
      || (ninth * 5 < cursor && cursor < ninth * 8)
      || (ninth * 7 < cursor && cursor < ninth * 9))
      {
        struct List *shield = AllocFullListElement();
        shield->Entity->Health = __ShieldObjectHealth;
        shield->Entity->Position->Column = cursor;
        shield->Entity->Position->Row = shildRow;
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

  /* draw player */
  GoToTerminalPosition(playerPosition->Row, playerPosition->Column);
  printf("%c", __PlayerAppearence);

  /*draw spacer */
  int i = 1;
  while(i <= terminalSize.ws_col)
  {
    GoToTerminalPosition(terminalSize.ws_row - 1, i);
    printf("_");
    i++;
  }

  /* draw score */
  GoToTerminalPosition(terminalSize.ws_row, 1);
  printf("<score: %d>", score);

  /* draw health */
  GoToTerminalPosition(terminalSize.ws_row, (int) terminalSize.ws_col / 2);
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
