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
    Initialize();               // prepare entity's and initial draw

    struct Player player;
    Invader invaders[_InvaderPerRow * _InvaderRowCount];
    struct Projectile projectiles[_MaximumProjectiles];
    struct Bomb bombs[_MaximumBombs];
    struct Shield shields[100]; 

    /* init player */
    Position playerPosition;
    playerPosition.Column = 0;
    playerPosition.Row = 0;
    player.Health = 3;
    player.Score = 0;
    player.Symbol = _PlayerAppearence;
    player.Position = &playerPosition;

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
        invaders[i].Direction = DOWN;
        invaders[i].SymbolOne = _InvaderAppearence;
        invaders[i].SymbolTwo = _InvaderAppearenceTwo;
        invaders[i].SymbolThree = _InvaderAppearenceThree;
        invaders[i].SymbolFour = _InvaderAppearenceFour;
        invaders[i].SymbolSwitch = ONE;
        i++;
    }

    /**/
    /**/

    /* GAMELOOP */
    while(true)
    {
        DrawInvaders(invaders); 
        refresh();
        
        if(key == KEY_ESC) break;
        key = inputThread->key;

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
    /*draw invaders*/
    int i = 0;
    while (i < (_InvaderPerRow * _InvaderRowCount))
    {
        if (invaders[i].SymbolSwitch)
            mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolOne);
        else
            mvaddch(invaders[i].Position->Row, invaders[i].Position->Column, invaders[i].SymbolTwo);
        
        i++;
    }
}

void DeleteChar(struct Position *pos)
{
    mvaddch(pos->Row, pos->Column, ' ');
}

/* ================================================================================================================= */
/* ====================================================== LOGIC ==================================================== */
/* ================================================================================================================= */
void Initialize(Invader *invaders)
{
}

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

void MoveInvaders(Invader *invaders)
{    
    // /* check direction */
    // ValidateInvaderDirection(invaders);   

    // while (invaderList != NULL)
    // {
    //     invaderList->Entity->SymbolSwitch = !invaderList->Entity->SymbolSwitch;
    //     /* delete old position */
    //     DeleteChar(invaderList->Entity->Position);

    //     /* move */
    //     if (invaderMoveForwart)
    //     {
    //         invaderList->Entity->Position->Row++;
    //         invaderList->Entity->SymbolSwitch = false;
    //     }
    //     else
    //     {
    //         if (!invaderDirection) /* true, left; flase, right */
    //         {
    //             invaderList->Entity->Position->Column = invaderList->Entity->Position->Column + _MoveHorizontalStep;
    //         }
    //         else
    //         {   
    //             invaderList->Entity->Position->Column = invaderList->Entity->Position->Column - _MoveHorizontalStep;
    //         }
    //     }

    //     /* switch Symbols */
    //     invaderList->Entity->SymbolSwitch = !invaderList->Entity->SymbolSwitch;

    //     /* next */
    //     invaderList = invaderList->Next;
    // }
    // invaderMoveForwart = false;
}

void ValidateInvaderDirection(Invader *invaders)
{
    // struct List *list = (struct List *)GetFirstElement(invaders);

    // if (list == NULL)
    //     return;

    // int min = list->Entity->Position->Column - _MoveHorizontalStep;
    // int max = list->Entity->Position->Column + _MoveHorizontalStep;

    // int i = 1;
    // while (list != NULL)
    // {
    //     if (list->Entity->Position->Column - _MoveHorizontalStep < min)
    //         min = list->Entity->Position->Column - _MoveHorizontalStep;

    //     if (list->Entity->Position->Column + _MoveHorizontalStep > max)
    //         max = list->Entity->Position->Column + _MoveHorizontalStep;

    //     list = list->Next;    
    //     if (i++ > _InvaderPerRow)
    //         break;
    // }

    // if (min <= 1 && invaderDirection == true)
    // {
    //     invaderDirection = !invaderDirection;
    //     bounceCounter++;
    // }
    // if (max >= COLS && invaderDirection == false)
    // {
    //     invaderDirection = !invaderDirection;
    //     bounceCounter++;
    // }
}
