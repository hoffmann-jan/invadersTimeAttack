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
struct list* AddElement(struct list *listElement, struct list *newElement);
struct list* InsertElement(struct list *listElement, struct list *newElement);
struct list* RemoveAndDestroyElement(struct list *removeElement);
struct list* GetFirstElement(struct list *listElement);
struct list* GetLastElement(struct list *listElement);

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

struct list* AddElement(struct list *listElement, struct list *newElement)
{
    if (listElement == NULL)
    {
      newElement->Previous = NULL;
      newElement->Next = NULL;
      return newElement;
    }
    else
    {
        listElement = GetLastElement(listElement);
        listElement->Next = newElement;
        listElement->Next->Previous = listElement;
        listElement->Next->Next = NULL;
    }

    return listElement;
}

struct list* InsertElement(struct list *listElement, struct list *newElement)
{
    if (listElement == NULL)
    {
      newElement->Previous = NULL;
      newElement->Next = NULL;
      return newElement;
    }
    else
    {
        listElement = GetFirstElement(listElement);
        listElement->Previous = newElement;
        listElement->Previous->Next = listElement;
        listElement->Previous->Previous = NULL;
    }

    return listElement;
}

struct list* RemoveAndDestroyElement(struct list *removeElement)
{
    struct list *returnElement = NULL;
  /* element in the middle of list */
  if (removeElement->Next != NULL && removeElement->Previous != NULL)
  {
    struct list *leftElement = removeElement->Previous;
    struct list *rightElement = removeElement->Next;

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
    struct list *rightElement = removeElement->Next;
    rightElement->Previous = NULL;

    returnElement = rightElement;
  }
  /* is last element */
  else if (removeElement == GetLastElement(removeElement))
  {
    struct list *leftElement = removeElement->Previous;
    leftElement->Next = NULL;

    returnElement = leftElement;
  }

  free(removeElement);
  return returnElement;
}

struct list* GetFirstElement(struct list *listElement)
{
  if (listElement == NULL)
        return NULL;
    while (listElement->Previous != NULL)
    {
        listElement = listElement->Previous;
    }
    return listElement;
}

struct list* GetLastElement(struct list *listElement)
{
  if (listElement == NULL)
        return NULL;
    while (listElement->Next != NULL)
    {
        listElement = listElement->Next;
    }
    return listElement;
}