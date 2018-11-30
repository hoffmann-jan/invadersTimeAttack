#include "program.h"

int bounceCounter = 1;
Direction lastDirection = LEFT;
unsigned long long frameCounter = 0;

int main(void)
{    
    /* start InputThread, special thanks to j.smolka for support <3 */
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
        projectiles[i].Symbol = _DisabledAppearence;
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
        bombs[i].Symbol = _DisabledAppearence;
        bombs[i].Direction = DOWN;
        bombs[i].Collision = true;
        i++;
    }

    /* init shields */
    i = 0;
    int cursor = 1;
    int ninth = (int) COLS / 9;
    /* set row for the shields */
    int shieldRow = (int) (LINES / 10) * 8;
    while (i < _MaximumShields)
    {
        Position * shieldPosition = (Position *)malloc(sizeof(Position));
        shieldPosition->Column = -1;
        shieldPosition->Row = -1;
        shields[i].Health = 5;
        if (cursor <= COLS)
        {
            while (cursor <= COLS)
            {
                if ( (ninth < cursor && cursor <= ninth * 2)
                || (ninth * 3 < cursor && cursor <= ninth * 4)
                || (ninth * 5 < cursor && cursor <= ninth * 6)
                || (ninth * 7 < cursor && cursor <= ninth * 8))
                {
                    (*shieldPosition).Column = cursor;
                    (*shieldPosition).Row = shieldRow;
                    cursor++;
                    break;
                }
                if (cursor > _MaximumShields)
                {
                    shields[i].Health = 0;
                    break;
                }
                cursor++;
            }
        }
        else
        {
            shields[i].Health = 0;
        }
        shields[i].Position = shieldPosition;
        shields[i].SymbolOne = _ShieldAppearence;
        shields[i].SymbolTwo = _ShieldAppearenceTwo;
        shields[i].SymbolThree = _ShieldAppearenceThree;
        shields[i].SymbolFour = _ShieldAppearenceFour;
        shields[i].SymbolSwitch = ONE;
        i++;
    }

    bool breakLoop = false;
    /* ================================================================================================================= */
    /* ====================================================== DRAW ===================================================== */
    /* ================================================================================================================= */
    while(true)
    {        

        DetectCollision(&player, invaders, projectiles, bombs, shields);
        DrawInvaders(invaders); 
        DrawPlayer(player);
        DrawProjectiles(projectiles);
        DrawScore(player);
        DrawHealth(player);
        DrawShields(shields);
        DrawBombs(bombs);
        refresh();
        Test(&player);

        key = inputThread->key;

        switch(key)
        {
            case KEY_ESC:
                breakLoop = true;
                break;
            case KEY_A:
            case KEY_a:
            case KEY_LEFT:
                mvaddch(player.Position->Row, player.Position->Column, ' ');
                player.Position->Column--;
                releaseThreadKey(inputThread);
                break;
            case KEY_RIGHT:
            case KEY_D:
            case KEY_d:
                mvaddch(player.Position->Row, player.Position->Column, ' ');
                player.Position->Column++;
                releaseThreadKey(inputThread);
                break;
            case KEY_Space:
                Shoot(projectiles, player);
                releaseThreadKey(inputThread);
                break;
        } 
        

        /* 1000000 = 1s */
        usleep(1000000 / _FramesPerSecond);
        frameCounter++;
        
        if((frameCounter % (_FramesPerSecond / 2) == 0)) //nach _FramesPerSecond Frames
        {
            MoveInvaders(invaders);    
            frameCounter = 0;
        }

        if((frameCounter % ((int)(_FramesPerSecond / 8)) == 0)) // 8x speed
        {
            MoveProjectiles(projectiles);           
        }

        //untere rechte ecke Frameinfo
        mvprintw(LINES - 4,COLS - 12, "            ");
        mvprintw(LINES - 4,COLS - 12, "f: %d, %d", shields[0].Position->Row, shields[0].Position->Column);
        mvprintw(LINES - 3,COLS - 12, "            ");
        mvprintw(LINES - 3,COLS - 12, "s: %d, %d", shields[1].Position->Row, shields[1].Position->Column);
        mvprintw(LINES - 2,COLS - 12, "            ");
        mvprintw(LINES - 2,COLS - 12, "Frame: %u", frameCounter);
        mvprintw(LINES - 1,COLS - 12, "            ");
        mvprintw(LINES - 1,COLS - 12, "Key: %d", key);
        
        if(breakLoop) break;
    }

    Dispose(player, invaders, projectiles, bombs, shields);
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

void Dispose(Player player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[])
{
    /* free game entities */
    for(int s = 0; s < _MaximumShields; s++)
    {
        free(shields[s].Position);
    }
    for(int i = 0; i < (_InvaderPerRow * _InvaderRowCount); i++)
    {
        free(invaders[i].Position);
    }
    for(int p = 0; p < _MaximumProjectiles; p++)
    {
        free(projectiles[p].Position);
    }
    for(int b = 0; b < _MaximumBombs; b++)
    {
        free(bombs[b].Position);
    }
    free(player.Position);

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
void DrawInvaders(Invader invaders[]) 
{
    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        if(!invaders[i].Health)
        {
            i++;
            continue;
        }

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

void DrawProjectiles(Projectile projectiles[])
{
    int i = 0;
    while (i < _MaximumProjectiles)
    {
        if (projectiles[i].Collision == false)
            mvaddch(projectiles[i].Position->Row, projectiles[i].Position->Column, projectiles[i].Symbol);
        i++;
    }
}

void DrawShields(Shield shields[])
{
    int i = 0;
    while (i < _MaximumShields)
    {
        if (shields[i].Health > 0)
        {
            switch (shields[i].SymbolSwitch)
            {
                case ONE:
                    mvaddch(shields[i].Position->Row, shields[i].Position->Column, shields[i].SymbolOne);
                    break;
                case TWO:
                    mvaddch(shields[i].Position->Row, shields[i].Position->Column, shields[i].SymbolTwo);
                    break;
                case THREE:
                    mvaddch(shields[i].Position->Row, shields[i].Position->Column, shields[i].SymbolThree);
                    break;
                case FOUR:
                    mvaddch(shields[i].Position->Row, shields[i].Position->Column, shields[i].SymbolFour);
                    break;            
            }
        }
        else
        {
            mvaddch(shields[i].Position->Row, shields[i].Position->Column, ' ');
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

void MoveInvaders(Invader invader[])
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

void ValidateInvaderDirection(Invader invader[])
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

void Shoot(Projectile projectiles[], Player player)
{
    int i = 0;
    while (i < _MaximumProjectiles)
    {
        if (projectiles[i].Collision == true)
        {
            projectiles[i].Symbol = _ProjectileAppearence;
            projectiles[i].Position->Column = player.Position->Column;
            projectiles[i].Position->Row = player.Position->Row - 1;
            projectiles[i].Collision = false;
            
            break;
        }

        i++;
    }
}

void MoveProjectiles(Projectile projectiles[])
{
    int i = 0;
    while (i < _MaximumProjectiles)
    {
        if (projectiles[i].Collision == false)
        {
            DeleteChar(projectiles[i].Position);
            projectiles[i].Position->Row--;     

            if (projectiles[i].Position->Row < 1)
                projectiles[i].Collision = true;
        }

        i++;
    }
}

void DetectCollision(Player *player, Invader invaders[], Projectile projectiles[], Bomb bombs[], Shield shields[])
{
    for(int p = 0; p < _MaximumProjectiles; p++)
    {
        if (projectiles[p].Collision == true)
            continue;

        bool hit = false;
        /* collision with a shield */
        for(int s = 0; s < _MaximumShields; s++)
        {
            if (shields[s].Health <= 0)
                continue;

            if (shields[s].Position->Row == projectiles[p].Position->Row
            && shields[s].Position->Column == projectiles[p].Position->Column)
            {
                DealShieldDamage(&shields[s]);
                projectiles[p].Collision = true;
                projectiles[p].Symbol = _DisabledAppearence;
                hit = true;
                break;
            }
        }

        if (hit)
        {
            continue;
        }

        /* collision with a invader */
        for(int i = 0; i < (_InvaderPerRow * _InvaderRowCount); i++)
        {
            if (invaders[i].Health == false)
                continue;
            if (invaders[i].Position->Row == projectiles[p].Position->Row
            && invaders[i].Position->Column == projectiles[p].Position->Column)
            {
                player->Score += 50;
                invaders[i].Health = false;
                projectiles[p].Collision = true;
                DeleteChar(projectiles[i].Position);
            }
       
        }
    }

}

void DealShieldDamage(Shield *shield)
{
    shield->Health--;

    switch (shield->Health)
    {
        case 4:
            shield->SymbolSwitch = ONE;
            break;
        case 3:
            shield->SymbolSwitch = TWO;
            break;
        case 2:
            shield->SymbolSwitch = THREE;
            break;
        case 1:
            shield->SymbolSwitch = FOUR;
            break;            
    }

    if (shield->Health == 0)
    {
        DeleteChar(shield->Position);
    }
}

void DrawScore(Player player)
{
    mvprintw(LINES - 2, 1, "<score: %d>", player.Score);
}

void DrawHealth(Player player)
{
    mvprintw(LINES - 2, (int)COLS / 2,"<health: %d>", player.Health);
}

void DrawBombs(Bomb bombs[])
{

}

/* DEBUG */
void Test(Player *ply)
{
    
}
