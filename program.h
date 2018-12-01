#ifndef INVADER_NCURSES_H
#define INVADER_NCURSES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

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
#define _BombAppearence '*'
#define _DisabledAppearence ' '
#define _PlayerAppearence 'A'
#define _ProjectileAppearence 'T'
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

#define _MoveHorizontalStep 1
// 
#define ClearTerminal() clear()
#define GoToTerminalPosition move

// ==================== FUNCTIONS =======================
/* Runtime */
void Initialize();
void Dispose(Player player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[]);
void SetUp();
void SetDown();
void GameLoop(InputThread *inputThread, int key, bool breakLoop, Player player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[]);
int RunGame(InputThread *inputThread);

/* draw support */
void ShowSplashScreen(InputThread *inputThread);
int ShowGameOverScreen(InputThread *inputThread, int score);
int PrintSplashScreen(InputThread *inputThread, char ** image, int imageLength, char ** message, int messageLength);
void Draw();
void DrawInvaders(Invader []);
void BuildShields(Shield shield[]);
void DrawShields(Shield shield[]);
void DrawPlayer(Player player);
void DrawBombs(Bomb bombs[]);

/* game logic*/
void GetNextPosition(Position *lastPosition, Position *newPosition, int listCount);
void MoveInvaders(Invader invader[]);
void MoveBombs(Bomb bombs[]);
void MoveProjectiles(Projectile projectiles[]);
void Shoot(Projectile projectiles[], Player player);
void ValidateInvaderDirection(Invader invader[]);
void DealShieldDamage(Shield *shield);
void DetectCollision(Player *player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[]);
void DrawScore(Player player);
void DrawHealth(Player player);
void DrawProjectiles(Projectile projectiles[]);
void Test(Player *ply);
bool IsGameOver(Player player, Invader invader[]);
void DropBomb(Invader invader[], Bomb bombs[]);

#endif
