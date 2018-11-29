#include "program.h"

/* globals */
// settings
// InputThread 

// entitys
// struct Invader invaders[_InvaderPerRow * _InvaderRowCount];


// 
bool invaderDirection = false;
bool invaderMoveForwart = false;
int bounceCounter = 1;

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
    struct Projectile projectiles[_MaximumProjectiles];
    struct Bomb bombs[_MaximumBombs];
    struct Shield shields[100]; 

    /* init player */
    Position * playerPosition = (Position *)malloc(sizeof(Position) + 1);;
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
        Position * invaderPosition = (Position *)malloc(sizeof(Position) + 1);
        
        int row = i % _InvaderPerRow;
        int col = i / _InvaderPerRow;

        invaderPosition->Column = _InvaderFirstRow + row + ( row * _InvaderHorizontalSpace);
        invaderPosition->Row = _InvaderFirstColumn + col + (col * _InvaderVerticalSpace);
        invaders[i].Position = invaderPosition;

        invaders[i].Health = true;
        invaders[i].Direction = DOWN;
        invaders[i].SymbolOne = _InvaderAppearence;
        invaders[i].SymbolTwo = _InvaderAppearenceTwo;
        invaders[i].SymbolThree = _InvaderAppearenceThree;
        invaders[i].SymbolFour = _InvaderAppearenceFour;
        invaders[i].SymbolSwitch = ONE;
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
        else if (key == KEY_A || key == KEY_a)
        {
            // DeleteChar(player.Position);
            mvaddch(player.Position->Row, player.Position->Column, ' ');
            player.Position->Column--;
        }
        else if (key == KEY_D || key == KEY_d) 
        {
            // DeleteChar(player.Position);
            mvaddch(player.Position->Row, player.Position->Column, ' ');
            player.Position->Column++;
        }

        /* 1000000 = 1s */
        usleep(1000000 / (_FramesPerSecond));
        frameCounter++;
        
        if((frameCounter % (_FramesPerSecond) == 0)) //nach _FramesPerSecond Frames
        {
            MoveInvaders(invaders);    
            frameCounter = 0;
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
        if (invaderMoveForwart)
        {
            invader[i].Position->Row++;
        }
        else
        {
            if (!invaderDirection) /* true, left; flase, right */
            {
                invader[i].Position->Column = invader[i].Position->Column + _MoveHorizontalStep;
            }
            else
            {   
                invader[i].Position->Column = invader[i].Position->Column - _MoveHorizontalStep;
            }
        }
        i++;
    }

    invaderMoveForwart = false;
}

void ValidateInvaderDirection(Invader *invader)
{
    int min = invader[0].Position->Column - _MoveHorizontalStep;
    int max = invader[0].Position->Column + _MoveHorizontalStep;

    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        if (invader[i].Position->Column - _MoveHorizontalStep < min)
            min = invader[i].Position->Column - _MoveHorizontalStep;

        if (invader[i].Position->Column + _MoveHorizontalStep > max)
            max = invader[i].Position->Column + _MoveHorizontalStep;

        if (i++ > _InvaderPerRow)
            break;
        i++;
    }

    if (min <= 1 && invaderDirection == true)
    {
        invaderDirection = !invaderDirection;
        bounceCounter++;
    }
    if (max >= COLS && invaderDirection == false)
    {
        invaderDirection = !invaderDirection;
        bounceCounter++;
    }

    if (bounceCounter >= 3)
    {
        invaderMoveForwart = true;
        bounceCounter = 1;
    }
}
