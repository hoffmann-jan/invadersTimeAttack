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
#define _BombAppearence 'v'
#define _PlayerAppearence '^'
#define _ProjectileAppearence '|'
#define _InvaderAppearence 'X'
#define _InvaderAppearenceTwo 'V'
#define _InvaderAppearenceThree 'W'
#define _InvaderAppearenceFour 'w'
#define _ShieldAppearence '#'
#define _ShieldAppearenceTwo '*'
#define _ShieldAppearenceThree '+'
#define _ShieldAppearenceFour '.'
#define _InvaderHorizontalSpace 5
#define _InvaderVerticalSpace 2
#define _InvaderFirstRow 1
#define _InvaderFirstColumn 1
#define _MaximumBombs 100
#define _MaximumShields 100
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
void DrawInvaders(Invader []);
void DrawScore();
void BuildShields();
void DrawPlayer();

/* game logic*/
void GetNextPosition(Position *lastPosition, Position *newPosition, int listCount);
void MoveInvaders(Invader invader[]);
void MoveProjectiles(Projectile projectiles[]);
void Shoot(Projectile projectiles[]);
void ValidateInvaderDirection(Invader invader[]);
void IncrementScore(int value);
void DealShieldDamage(Shield shield);
void DetectCollision(Player player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[]);

#endif
