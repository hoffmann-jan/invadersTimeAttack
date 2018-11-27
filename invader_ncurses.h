#ifndef INVADER_NCURSES_H
#define INVADER_NCURSES_H

#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>

#include "invader_ncurses.h"
#include "InputThread.h"
#include "List.h"
#include "Entity.h"
#include "Position.h"

// =================== DEFINITIONS =====================

// Settings
#define _FramesPerSecond 30

// GameElements
#define _InvaderPerRow 11
#define _InvaderRowCount 5
#define _InvaderAppearence 'V'
#define _InvaderAppearenceTwo 'W'
#define _InvaderHorizontalSpace 5
#define _InvaderVerticalSpace 2
#define _InvaderFirstRow 2
#define _InvaderFirstColumn 2

#define _MoveHorizontalStep 2
// 
#define ClearTerminal() clear()
#define GoToTerminalPosition move

// ==================== FUNCTIONS =======================
/* Runtime */
void Initialize();
void Dispose();
void SetUp();           

/* draw support */
void ShowSplashScreen();
void Draw();
void PrintChar();

/* game logic*/
void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount);
void MoveInvaders();
void ValidateInvaderDirection();

#endif



