#include "program.h"

int bounceCounter = 1;
Direction lastDirection = LEFT;
unsigned long long frameCounter = 0;

int main(void)
{    
    /* start Inputthread, special thanks to j.smolka for support <3 */
    InputThread *inputThread = threadAlloc();
    threadStart(inputThread);
    int key = 0;

    SetUp();                    // prepare tec - spezifics
    ShowSplashScreen();
    
    /* setup vars */
    Player player;
    Invader invaders[_InvaderPerRow * _InvaderRowCount];
    Projectile projectiles[_MaximumProjectiles];
    Bomb bombs[_MaximumBombs];
    Shield shields[_MaximumShields]; 

    /* init player */
    Position * playerPosition = (Position *)malloc(sizeof(Position));;
    playerPosition->Column = COLS / 2;
    playerPosition->Row = LINES - 5;
    player.Health = 3;
    player.Score = 0;
    player.Symbol = _PlayerAppearence;
    player.Position = playerPosition;
    
    /* init invaders */
    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        Position * invaderPosition = (Position *)malloc(sizeof(Position));
        
        int row = i % _InvaderPerRow;
        int col = i / _InvaderPerRow;

        invaderPosition->Column = _InvaderFirstRow + row + ( row * _InvaderHorizontalSpace);
        invaderPosition->Row = _InvaderFirstColumn + col + (col * _InvaderVerticalSpace);
        invaders[i].Position = invaderPosition;

        invaders[i].Health = true;
        invaders[i].Direction = LEFT;
        invaders[i].SymbolOne = _InvaderAppearence;
        invaders[i].SymbolTwo = _InvaderAppearenceTwo;
        invaders[i].SymbolThree = _InvaderAppearenceThree;
        invaders[i].SymbolFour = _InvaderAppearenceFour;
        invaders[i].SymbolSwitch = ONE;
        i++;
    }

    /* init projectile array */
    i = 0;
    while (i < _MaximumProjectiles)
    {
        Position * projectilePosition = (Position *)malloc(sizeof(Position));

        projectiles[i].Position = projectilePosition;
        projectiles[i].Symbol = _ProjectileAppearence;
        projectiles[i].Direction = UP;
        projectiles[i].Collision = true;
        i++;
    }

    /* init bomb array */
    i = 0;
    while (i < _MaximumBombs)
    {
        Position * bombPosition = (Position *)malloc(sizeof(Position));

        bombs[i].Position = bombPosition;
        bombs[i].Symbol = _BombAppearence;
        bombs[i].Direction = DOWN;
        bombs[i].Collision = true;
        i++;
    }

    /* init shields */
    i = 0;
    while (i <= _MaximumShields)
    {
        Position * shieldPosition = (Position *)malloc(sizeof(Position));

        shields[i].Position = shieldPosition;
        shields[i].SymbolOne = _ShieldAppearence;
        shields[i].SymbolTwo = _ShieldAppearenceTwo;
        shields[i].SymbolThree = _ShieldAppearenceThree;
        shields[i].SymbolFour = _ShieldAppearenceFour;
        shields[i].SymbolSwitch = ONE;
        shields[i].Health = 5;
        i++;
    }

    /* GAMELOOP */
    while(true)
    {
        DrawInvaders(invaders); 
        DrawPlayer(player);
        refresh();

        key = inputThread->key;

        if (key == KEY_ESC)
        {
            break;
        }
        else if (key == KEY_A || key == KEY_a || key == KEY_LEFT)
        {
            // DeleteChar(player.Position);
            mvaddch(player.Position->Row, player.Position->Column, ' ');
            player.Position->Column--;
        }
        else if (key == KEY_D || key == KEY_d || key == KEY_RIGHT) 
        {
            // DeleteChar(player.Position);
            mvaddch(player.Position->Row, player.Position->Column, ' ');
            player.Position->Column++;
        }
        else if (key == KEY_Space)
        {
            
        }

        /* 1000000 = 1s */
        usleep(1000000 / _FramesPerSecond);
        frameCounter++;
        
        if((frameCounter % _FramesPerSecond == 0)) //nach _FramesPerSecond Frames
        {
            MoveInvaders(invaders);    
            frameCounter = 0;
        }

        if((frameCounter % ((int)(_FramesPerSecond / 2)) == 0)) // double speed
        {
            MoveProjectiles(invaders); 
        }

        //untere rechte ecke Frameinfo
        mvprintw(LINES - 4,COLS - 12, "            ");
        mvprintw(LINES - 4,COLS - 12, "l: %d, %d", invaders[0].Position->Row, invaders[0].Position->Column);
        mvprintw(LINES - 3,COLS - 12, "            ");
        mvprintw(LINES - 3,COLS - 12, "r: %d, %d", invaders[(_InvaderPerRow * _InvaderRowCount) - 1].Position->Row, invaders[(_InvaderPerRow * _InvaderRowCount) - 1].Position->Column);
        mvprintw(LINES - 2,COLS - 12, "            ");
        mvprintw(LINES - 2,COLS - 12, "Frame: %u", frameCounter);
        mvprintw(LINES - 1,COLS - 12, "            ");
        mvprintw(LINES - 1,COLS - 12, "Key: %d", key);
        
    }

    Dispose();
    return EXIT_SUCCESS;
}


void SetUp()
{
    //ncurses options
    initscr();              /* Start ncurses mode */
    //raw();                /* disable line buffering - no Endline or CR needed */
    cbreak();               /* like raw, but enables CTRL+C */
    keypad(stdscr, TRUE);   /* enable keys like F1, arrowkeys ... */
    noecho();               /* disable input buffer on screen e.g. escape stuff*/
    curs_set(0);            /* disable cursor blinki blinki */
    //nodelay(stdscr, 1);   /* */
    srand(time(NULL));
}

void Dispose()
{
    //ncurse release
    refresh();
    endwin();              /* stop ncurses mode IMPORTANT! ;D*/
    ClearTerminal();
}

void ShowSplashScreen()
{
    ClearTerminal();
  
    int fifth = (int) LINES / 5;
    int startRow = fifth * 2;
    int startColumn = ((int) COLS / 2) - 25;

    mvprintw(startRow++, startColumn, "    _____   ___    _____    ____  __________  _____");
    mvprintw(startRow++, startColumn, "   /  _/ | / / |  / /   |  / __ \\/ ____/ __ \\/ ___/");
    mvprintw(startRow++, startColumn, "   / //  |/ /| | / / /| | / / / / __/ / /_/ /\\__ \\ ");
    mvprintw(startRow++, startColumn, " _/ // /|  / | |/ / ___ |/ /_/ / /___/ _, _/___/ / ");
    mvprintw(startRow++, startColumn, "/___/_/ |_/  |___/_/  |_/_____/_____/_/ |_|/____/  ");
    mvprintw(startRow, startColumn, "DEBUG_INFO::lines: %d, cols: %d", LINES, COLS);
    mvprintw(fifth * 3, startColumn + 10, "press any key to start .. ");
    
    getch();

    /* clear screen */
    ClearTerminal();
}

/* ================================================================================================================= */
/* ====================================================== DRAW ===================================================== */
/* ================================================================================================================= */
void DrawInvaders(Invader *invaders) 
{
    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        switch (invaders[i].SymbolSwitch)
        {
            case ONE:
                mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolOne);
                break;
            case TWO:
                mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolTwo);
                break;
            case THREE:
                mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolThree);
                break;
            case FOUR:
                mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolFour);
                break;
        }
        i++;
    }
}

void DrawPlayer(Player player)
{
    mvaddch(player.Position->Row, player.Position->Column, player.Symbol);
}

void DeleteChar(Position *position)
{
    mvaddch(position->Row, position->Column, ' ');
}

/* ================================================================================================================= */
/* ====================================================== LOGIC ==================================================== */
/* ================================================================================================================= */

void GetNextPosition(Position *lastPosition, Position *newPosition, int listCount)
{
    /* first invader */
    if (lastPosition == NULL)
    {
        newPosition->Row = _InvaderFirstRow;
        newPosition->Column = _InvaderFirstColumn;
        return;
    }

    /* ohters */
    /* new row */
    if (listCount % _InvaderPerRow == 0)
    {
        newPosition->Row = lastPosition->Row + _InvaderVerticalSpace;
        newPosition->Column = _InvaderFirstColumn;
    }
    /* next column */
    else
    {
        newPosition->Row = lastPosition->Row;
        newPosition->Column = lastPosition->Column + _InvaderHorizontalSpace;
    }
}

void MoveInvaders(Invader *invader)
{    
    /* check and set direction */
    ValidateInvaderDirection(invader);   

    /* switch appearence */
    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        switch (invader[i].SymbolSwitch)
        {
            case ONE:
                invader[i].SymbolSwitch = TWO;
                break;
            case TWO:
                invader[i].SymbolSwitch = THREE;
                break;
            case THREE:
                invader[i].SymbolSwitch = FOUR;
                break;
            case FOUR:
                invader[i].SymbolSwitch = ONE;
                break;
        }

        /* delete old position */
        DeleteChar(invader[i].Position);

        /* move */
        switch (invader[i].Direction)
        {
            case LEFT:
                invader[i].Position->Column = invader[i].Position->Column - _MoveHorizontalStep;
                break;
            case RIGTH:
                invader[i].Position->Column = invader[i].Position->Column + _MoveHorizontalStep;
                break;
            case DOWN:
                invader[i].Position->Row++;
                invader[i].Direction = LEFT;
            break;
            default:
                break;
        }
        
        i++;
    }
}

void ValidateInvaderDirection(Invader *invader)
{
    int min = invader[0].Position->Column - _MoveHorizontalStep;
    int max = invader[0].Position->Column + _MoveHorizontalStep;

    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        if (invader[i].Direction == DOWN)
            invader[i].Direction = lastDirection;
        else
            lastDirection = invader[i].Direction;


        if (invader[i].Position->Column - _MoveHorizontalStep < min)
            min = invader[i].Position->Column - _MoveHorizontalStep;

        if (invader[i].Position->Column + _MoveHorizontalStep > max)
            max = invader[i].Position->Column + _MoveHorizontalStep;

        if (i++ > _InvaderPerRow)
            break;
        i++;
    }

    if ((min <= 1 && invader[i].Direction == LEFT)
    || (max >= COLS && invader[i].Direction == RIGTH))
    {
        bounceCounter++;
    }

    i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        if (min <= 1 && invader[i].Direction == LEFT)
        {
            invader[i].Direction = RIGTH;
        }
        if (max >= COLS && invader[i].Direction == RIGTH)
        {
            invader[i].Direction = LEFT;
        }

        if (bounceCounter >= 3)
        {
            invader[i].Direction = DOWN;
        }
        i++;
    }
    if (bounceCounter >= 3)    
        bounceCounter = 1;

}

void Shoot()
{
//   struct List * projectile = (struct List *)AllocFullListElement();

//   projectile->Entity->Position->Column = playerPosition->Column;
//   projectile->Entity->Position->Row = playerPosition->Row - 1;
//   projectile->Entity->SymbolOne = __ProjectileAppearence;
//   projectile->Entity->SymbolTwo = __ProjectileAppearence;
//   projectile->Entity->SymbolSwitch = true;
//   PrintChar(projectile);

//   projectiles = (struct List *)AddElement(projectiles, projectile);
}

void MoveProjectiles()
{
//   struct List *projectileList = (struct List *)GetFirstElement(projectiles);
//   while (projectileList != NULL)
//   {
//     DeleteChar(projectileList->Entity->Position);
//     if (projectileList->Entity->Position->Row <= 1)
//     {
//       projectileList = (struct List *)RemoveAndDestroyElement(projectileList);
//     }
//     else
//     {
//       /* move */
//       projectileList->Entity->Position->Row--;

//       /* draw */
//       PrintChar(projectileList);
      
//       /* next */
//       projectileList = projectileList->Next;
//     }
//   }
}

void DetectCollision()
{
//   struct List *projectile = (struct List *)GetFirstElement(projectiles);
//   struct List *invader = (struct List *)GetFirstElement(invaders);
//   struct List *shield = (struct List *)GetFirstElement(shieldObjects);

//   while(projectile != NULL)
//   {
//     bool hit = false;
//     /* collision with a shield */
//     while(shield != NULL)
//     {
//       if (shield->Entity->Position->Row == projectile->Entity->Position->Row
//       && shield->Entity->Position->Column == projectile->Entity->Position->Column)
//       {
//         DealShieldDamage(shield);
//         hit = true;
//         break;
//       }
//       shield = shield->Next;
//     }
//     if (hit)
//     {
//       projectile = (struct List *)RemoveAndDestroyElement(projectile);
//       break;
//     }
//     /* collision with a invader */
//     while(invader != NULL)
//     {
//       if (invader->Entity->Position->Row == projectile->Entity->Position->Row
//       && invader->Entity->Position->Column == projectile->Entity->Position->Column)
//       {
//         //invader = (struct List *)RemoveAndDestroyElement(invader);
//         //projectile = (struct List *)RemoveAndDestroyElement(projectile);
//         IncrementScore(50);
//         break;
//       }
//       invader = invader->Next;
//     }
//     projectile = projectile->Next;
//   }

}

void DealShieldDamage(Shield shield)
{
//   shield->Entity->Health = shield->Entity->Health - 1;
//   if (shield->Entity->Health == 0)
//   {
//     DeleteChar(shield->Entity->Position);
//     //shield = (struct List *)RemoveAndDestroyElement(shield);
//   }
}

void IncrementScore(int value)
{
//   score += value;
//   DrawScore();
}

void DrawScore()
{
//   GoToTerminalPosition(terminalSize.ws_row - 2, 1);
//   printf("<score: %d>", score);
}

void DrawHealth()
{
//   GoToTerminalPosition(terminalSize.ws_row - 2, (int) terminalSize.ws_col / 2);
//   printf("<health: %d>", playerHealth);
}
