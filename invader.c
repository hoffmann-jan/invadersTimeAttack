/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add bool */ 
#include <stdbool.h>
/* malloc etc. */
#include <stdlib.h>

/* define terminal functions */
#define ClearTerminal() printf("\033[H\033[J")
#define GoToTerminalPosition(x,y) printf("\033[%d;%dH", (x), (y))
/* define Invader dimension */
#define __InvaderPerRow 11
#define __InvaderRowCount 5
/* define inital values */
#define __InitialPlayerHealth 3
#define __InvaderHorizontalSpace 4
#define __InvaderVerticalSpace 2
#define __InvaderFirstColumn 0
#define __InvaderFirstRow 0
#define __ShieldObjectHealth 5

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

struct List 
{
  struct Entity *Entity;
  struct List *Next;
  struct List *Previous;
};

/* List functions */
struct List* AddElement(struct List *ListElement, struct List *newElement);
struct List* InsertElement(struct List *ListElement, struct List *newElement);
struct List* RemoveAndDestroyElement(struct List *removeElement);
struct List* GetFirstElement(struct List *ListElement);
struct List* GetLastElement(struct List *ListElement);
int GetListCount(struct List *ListElement);

/* draw support */
void ShowSplashScreen();
void Draw();

/* helpers */
void Init();
void FreeAll();
void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount);

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
struct Position *playerPos = NULL;

int main (void)
{
  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &terminalSize);

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
  /* create invaders */
  while ( GetListCount(invaders) < (__InvaderPerRow * __InvaderRowCount) )
  {
    /* allocate invader */
    struct List * listElement = (struct List*)malloc( sizeof(struct List));
    struct Entity *invader =  (struct Entity*)malloc( sizeof(struct Entity));
    struct Position *position =  (struct Position*)malloc( sizeof(struct Position));

    /* set properties */
    GetNextPosition(GetLastElement(invaders)->Entity->Position, position, GetListCount(invaders));

    invader->SymbolOne = 'V';
    invader->SymbolTwo = 'W';
    invader->SymbolSwitch = false;

    /* stack, link */
    invader->Position = position;
    listElement->Entity = invader;

    /* add to List */
    invaders = AddElement(invaders, listElement);
  }

  /* place objects */

  /* initialise player and score */
  playerHealth = __InitialPlayerHealth;
  score = 0;
}

struct List* AddElement(struct List *ListElement, struct List *newElement)
{
    if (ListElement == NULL)
    {
      newElement->Previous = NULL;
      newElement->Next = NULL;
      return newElement;
    }
    else
    {
        ListElement = GetLastElement(ListElement);
        ListElement->Next = newElement;
        ListElement->Next->Previous = ListElement;
        ListElement->Next->Next = NULL;
    }

    return ListElement;
}

struct List* InsertElement(struct List *ListElement, struct List *newElement)
{
    if (ListElement == NULL)
    {
      newElement->Previous = NULL;
      newElement->Next = NULL;
      return newElement;
    }
    else
    {
        ListElement = GetFirstElement(ListElement);
        ListElement->Previous = newElement;
        ListElement->Previous->Next = ListElement;
        ListElement->Previous->Previous = NULL;
    }

    return ListElement;
}

struct List* RemoveAndDestroyElement(struct List *removeElement)
{
    struct List *returnElement = NULL;
  /* element in the middle of List */
  if (removeElement->Next != NULL && removeElement->Previous != NULL)
  {
    struct List *leftElement = removeElement->Previous;
    struct List *rightElement = removeElement->Next;

    leftElement->Next = rightElement;
    rightElement->Previous = leftElement;

    returnElement = rightElement;
  }
  /* element stand alone */
  else if (removeElement->Next == NULL && removeElement->Previous == NULL)
  {
    returnElement = NULL;
  }
  /* is first element */
  else if (removeElement == GetFirstElement(removeElement))
  {
    struct List *rightElement = removeElement->Next;
    rightElement->Previous = NULL;

    returnElement = rightElement;
  }
  /* is last element */
  else if (removeElement == GetLastElement(removeElement))
  {
    struct List *leftElement = removeElement->Previous;
    leftElement->Next = NULL;

    returnElement = leftElement;
  }

  free(removeElement->Entity->Position);
  free(removeElement->Entity);
  free(removeElement);
  return returnElement;
}

struct List* GetFirstElement(struct List *ListElement)
{
  if (ListElement == NULL)
        return NULL;
    while (ListElement->Previous != NULL)
    {
        ListElement = ListElement->Previous;
    }
    return ListElement;
}

struct List* GetLastElement(struct List *ListElement)
{
  if (ListElement == NULL)
        return NULL;
    while (ListElement->Next != NULL)
    {
        ListElement = ListElement->Next;
    }
    return ListElement;
}

int GetListCount(struct List *ListElement)
{
  if (ListElement == NULL)
    return 0;

  ListElement = GetFirstElement(ListElement);

  int count = 1;
  while (ListElement->Next != NULL)
  {
    ListElement = ListElement->Next;
    count++;
  }

  return count;
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

