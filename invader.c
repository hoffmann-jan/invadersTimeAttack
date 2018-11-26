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
/* strcpy */
#include <string.h>

/**/
#include "myconio.h"
#include "List.h"
#include "Entity.h"
#include "Position.h"
#include "InputThread.h"

/* define terminal functions */
#define ClearTerminal() printf("\033[H\033[J")
#define GoToTerminalPosition(row, col) printf("\033[%d;%dH", (row), (col))
#define HideCursor() printf("\e[?25l") 
#define ShowCursor() printf("\e[?25h")
#define ANSI_Color_Reset "\x1b[0m"
#define ANSI_Color_Black "\x1b[30m"
#define ANSI_Color_Red "\x1b[31m"
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
/* debugmode */
#define DEBUG_MODE_ENABLED //Comment to dissable

#ifdef DEBUG_MODE_ENABLED
#define LOG(format, ...) printf(format,__VA_ARGS)
int _DEBUG_MODE = 1;
#else
#define LOG(format, ...)
int _DEBUG_MODE = 0;
#endif

/* debugvariablen */
//enable step by step loop
int _PRESSED_BUTTON = 0;
int _DEBUG_FRAMECOUNTER = 0;

/* draw support */
void ShowSplashScreen();
int ShowGameOverScreen();
int PrintSplashScreen(char ** image, int imageLength, char ** message, int messageLength);
void Draw();

/* helpers */
void Init();

void FreeAll();
void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount);
void BuildShields();
void PrintChar(struct List *object);
void DeleteChar(struct Position * position);
void DrawPlayer();
void DrawScore();
void DrawHealth();
void IncrementScore(int value);


/* game logic */
void MoveInvaders();
void MoveProjectiles();
void MovePlayer(int direction);
void RemoveDefeatedEntities();
void DetectCollision();
void Shoot();
void ValidateInvaderDirection();
void DealShieldDamage(struct List *shield);
void GameLoop();
int RunGame();

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
  while (true)
  {
    _PRESSED_BUTTON = 0;
    if(1 == RunGame())
      break;
  }

  printf(ANSI_Color_Reset);
  ShowCursor();
  return 0;
}

int PrintSplashScreen(char ** image, int imageLength, char ** message, int messageLength)
{
  ClearTerminal();
  printf(ANSI_Color_Reset);
  int fifth = (int) terminalSize.ws_row / 5;
  int startRow = fifth * 2;
  int startColumn = ((int) terminalSize.ws_col / 2) - 25;

  for (int i = 0; i < imageLength; i++)
  {
    GoToTerminalPosition(startRow + i, startColumn);
    printf("%s", image[i]);
  }
  for(int i = imageLength - 1; i >= 0; i--)
  {
      free(image[i]);
  }
  free(image);

  for (int m = 0; m < messageLength; m++)
  {
    GoToTerminalPosition(fifth * 4 + m, startColumn + 1);
    printf("%s", message[m]);
  }
  for(int i = messageLength - 1; i >= 0; i--)
  {
      free(message[i]);
  }
  free(message);

  char result = getch();

  /* clear screen */
  ClearTerminal();
  /* move cursor to top left */
  GoToTerminalPosition(1, 1);

  if (result == 'q')
    return 1;
  else
    return 0;
}

void Init()
{
  /* create invaders */
  while ( GetListCount(invaders) < (__InvaderPerRow * __InvaderRowCount) )
  {
    /* allocate invader */
    struct List * listElement = (struct List *)AllocFullListElement();

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
    invaders = (struct List *)AddElement(invaders, listElement);
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
        struct List *shield = (struct List *)AllocFullListElement();
        shield->Entity->Health = __ShieldObjectHealth;
        shield->Entity->Position->Column = cursor;
        shield->Entity->Position->Row = shieldRow;
        shield->Entity->SymbolOne = __ShieldObjectAppearence;
        shield->Entity->SymbolTwo = __ShieldObjectAppearenceLowHealth;
        shield->Entity->SymbolSwitch = true;

        shieldObjects = (struct List *)AddElement(shieldObjects ,shield);
      }

    cursor++;
  }
}

void Draw()
{
  /* draw invaders */
  struct List *invaderList = (struct List *)GetFirstElement(invaders);
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
  struct List *shields = (struct List *)GetFirstElement(shieldObjects);
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
  struct List *projectileList = (struct List *)GetFirstElement(projectiles);
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
  DrawScore();

  /* draw health */
  DrawHealth();
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
  struct List *invaderList = (struct List *)GetFirstElement(invaders);

  /* check direction */
  ValidateInvaderDirection();   
  
  while (invaderList != NULL)
  {
    /* delete old position */
    DeleteChar(invaderList->Entity->Position);

    /* move */
    if (invaderMoveForwart)
    {
      invaderList->Entity->Position->Row++;
      invaderList->Entity->SymbolSwitch = false;
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
    invaderList->Entity->SymbolSwitch = !invaderList->Entity->SymbolSwitch;

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
        if (playerPosition->Column - 1 >= 1)
          playerPosition->Column = playerPosition->Column - 1;
        break;
      case 67: /* right */
        if (playerPosition->Column + 1 <= terminalSize.ws_col)
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
  struct List *list = (struct List *)GetFirstElement(invaders);

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
  struct List * projectile = (struct List *)AllocFullListElement();

  projectile->Entity->Position->Column = playerPosition->Column;
  projectile->Entity->Position->Row = playerPosition->Row - 1;
  projectile->Entity->SymbolOne = __ProjectileAppearence;
  projectile->Entity->SymbolTwo = __ProjectileAppearence;
  projectile->Entity->SymbolSwitch = true;
  PrintChar(projectile);

  projectiles = (struct List *)AddElement(projectiles, projectile);
}

void MoveProjectiles()
{
  struct List *projectileList = (struct List *)GetFirstElement(projectiles);
  while (projectileList != NULL)
  {
    DeleteChar(projectileList->Entity->Position);
    if (projectileList->Entity->Position->Row <= 1)
    {
      projectileList = (struct List *)RemoveAndDestroyElement(projectileList);
    }
    else
    {
      /* move */
      projectileList->Entity->Position->Row--;

      /* draw */
      PrintChar(projectileList);
      
      /* next */
      projectileList = projectileList->Next;
    }
  }
}

void DetectCollision()
{
  struct List *projectile = (struct List *)GetFirstElement(projectiles);
  struct List *invader = (struct List *)GetFirstElement(invaders);
  struct List *shield = (struct List *)GetFirstElement(shieldObjects);

  while(projectile != NULL)
  {
    bool hit = false;
    /* collision with a shield */
    while(shield != NULL)
    {
      if (shield->Entity->Position->Row == projectile->Entity->Position->Row
      && shield->Entity->Position->Column == projectile->Entity->Position->Column)
      {
        DealShieldDamage(shield);
        hit = true;
        break;
      }
      shield = shield->Next;
    }
    if (hit)
    {
      projectile = (struct List *)RemoveAndDestroyElement(projectile);
      break;
    }
    /* collision with a invader */
    while(invader != NULL)
    {
      if (invader->Entity->Position->Row == projectile->Entity->Position->Row
      && invader->Entity->Position->Column == projectile->Entity->Position->Column)
      {
        //invader = (struct List *)RemoveAndDestroyElement(invader);
        //projectile = (struct List *)RemoveAndDestroyElement(projectile);
        IncrementScore(50);
        break;
      }
      invader = invader->Next;
    }
    projectile = projectile->Next;
  }

}

void DealShieldDamage(struct List *shield)
{
  shield->Entity->Health = shield->Entity->Health - 1;
  if (shield->Entity->Health == 0)
  {
    DeleteChar(shield->Entity->Position);
    //shield = (struct List *)RemoveAndDestroyElement(shield);
  }
}

void IncrementScore(int value)
{
  score += value;
  DrawScore();
}

void DrawScore()
{
  GoToTerminalPosition(terminalSize.ws_row - 1, 1);
  printf("<score: %d>", score);
}

void DrawHealth()
{
  GoToTerminalPosition(terminalSize.ws_row - 1, (int) terminalSize.ws_col / 2);
  printf("<health: %d>", playerHealth);
}

int ShowGameOverScreen()
{
  int imageLength = 5;
  char ** image = (char **)malloc(sizeof(char *) * imageLength);

  for (int i = 0; i < imageLength; i++)
  {
    image[i] = malloc(sizeof(char) * 55 + 1 );
  }

  strcpy(image[0], "  ____    _    __  __ _____    _____     _______ ____  ");
  strcpy(image[1], " / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\ ");
  strcpy(image[2], "| |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) |");
  strcpy(image[3], "| |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ < ");
  strcpy(image[4], " \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_\\");

  int messageLength = 3;
  char ** message = (char **)malloc(sizeof(char *) * messageLength);

  for (int i = 0; i < messageLength; i++)
  {
    message[i] = malloc(sizeof(char) * 90 + 1 );
  }

  char *scoreString = (char*)malloc(90 * sizeof(char));
  sprintf(scoreString, "%s %d", "your score: ", score);
  strcpy(message[0], scoreString);
  strcpy(message[1], "");
  strcpy(message[2], "press any key to start .. or 'q' to quit");

  return PrintSplashScreen(image, imageLength, message, messageLength);
}

void FreeAll()
{

}

void ShowSplashScreen()
{
  int imageLength = 5;
  char ** image = (char **)malloc(sizeof(char *) * imageLength);

  for (int i = 0; i < imageLength; i++)
  {
    image[i] = malloc(sizeof(char) * 51 + 1 );
  }

  strcpy(image[0], "    _____   ___    _____    ____  __________  _____");
  strcpy(image[1], "   /  _/ | / / |  / /   |  / __ \\/ ____/ __ \\/ ___/");
  strcpy(image[2], "   / //  |/ /| | / / /| | / / / / __/ / /_/ /\\__ \\ ");
  strcpy(image[3], " _/ // /|  / | |/ / ___ |/ /_/ / /___/ _, _/___/ / ");
  strcpy(image[4], "/___/_/ |_/  |___/_/  |_/_____/_____/_/ |_|/____/  ");

  int messageLength = 1;
  char ** message = (char **)malloc(sizeof(char *) * messageLength);
  message[0] = malloc(sizeof(char) * 26 + 1 );
  strcpy(message[0], "press any key to start .. ");

  PrintSplashScreen(image, imageLength, message, messageLength);
}

void GameLoop()
{
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
      _PRESSED_BUTTON = getch();
      MovePlayer(_PRESSED_BUTTON);
      if (_PRESSED_BUTTON == 32) Shoot();
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
      DetectCollision();
      printf(ANSI_Color_Black);
    }

    if (_DEBUG_FRAMECOUNTER % (__FramesPerSecond / 3) == 0)
    {
      printf(ANSI_Color_Reset);
      MoveProjectiles();
      DetectCollision();
      printf(ANSI_Color_Black);
    }
    
    if (_PRESSED_BUTTON == 10) /* ENTER TO EXIT DEBUG */
    {
      ClearTerminal();
      break;
    }
  }
  /* end game loop */
}

int RunGame()
{
  /* get terminal size */
  ioctl(0, TIOCGWINSZ, &terminalSize);

  /* welcome window */
  ShowSplashScreen();
  HideCursor();

  /* game initialisation */
  Init();
  
  ClearTerminal();
  Draw();
  printf(ANSI_Color_Black);

  /*initalite input thread */
  //InputThread *thread = threadAlloc();
  //int key;
  //threadStart(thread);

  GameLoop();

  return ShowGameOverScreen();
}
