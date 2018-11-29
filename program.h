#ifndef INVADER_NCURSES_H
#define INVADER_NCURSES_H

#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>

#include "InputThread.h"
#include "Position.h"
#include "Projectile.h"
#include "Player.h"
#include "Bomb.h"
#include "Shield.h"
#include "Invader.h"
#include "Enums.h"

// =================== DEFINITIONS =====================

// Settings
#define _FramesPerSecond 30

// GameElements
#define _InvaderPerRow 11
#define _InvaderRowCount 5
#define _PlayerAppearence '^'
#define _InvaderAppearence 'X'
#define _InvaderAppearenceTwo 'V'
#define _InvaderAppearenceThree 'W'
#define _InvaderAppearenceFour 'w'
#define _InvaderHorizontalSpace 5
#define _InvaderVerticalSpace 2
#define _InvaderFirstRow 1
#define _InvaderFirstColumn 1
#define _MaximumBombs 100
#define _MaximumProjectiles 20

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
int ShowGameOverScreen();
int PrintSplashScreen(char ** image, int imageLength, char ** message, int messageLength);
void Draw();
void DrawInvaders(Invader *);
void BuildShields();
void DrawPlayer();

/* game logic*/
void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount);
void MoveInvaders();
void ValidateInvaderDirection();

#endif
