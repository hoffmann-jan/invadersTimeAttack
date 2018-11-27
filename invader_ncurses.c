#include "invader_ncurses.h"
#include "InputThread.h"
#include "Key.h"

/* globals */
// settings

// entitys
struct List *invaders = NULL;
bool invaderDirection = false;
bool invaderMoveForwart = false;
int bounceCounter = 1;

int frameCounter = 0;

int main(void)
{    
    /* starte Inputthread , danke an Julian für den support <3 */
    InputThread *inputThread = threadAlloc();
    threadStart(inputThread);
    int key = 0;


    SetUp();                    // prepare tec - spezifics
    ShowSplashScreen();
    Initialize();               // prepare entity's and initial draw

    /* GAMELOOP */
    while(true)
    {
        if(key == KEY_ESC) break;
        key = inputThread->key;

        /* 1000000 = 1s */
        usleep(1000000 / _FramesPerSecond);
        frameCounter++;
        
        Draw(); 

        if((frameCounter % (_FramesPerSecond / 1) == 0)) //nach 1 Sekunde
        {
            
            MoveInvaders();    
            //frameCounter = 0;
            
        }

        //obere Rechte ecke Frameinfo
        move(1,COLS - 10);
        printw("          ");
        move(1,COLS - 10);
        printw("Frame: %d", frameCounter);

        move(2,COLS - 10);
        printw("          ");
        move(2,COLS - 10);
        printw("Key: %d", key);
        

        refresh();
    }

    ClearTerminal();
    Dispose();
    return EXIT_SUCCESS;
}


void SetUp()
{
    //ncurses options
    initscr();              /* Start ncurses mode */
    //raw();                /* disable line buffering - no Endline or CR needed */
    cbreak();               /* like raw, but enables CTRL+C */
    keypad(stdscr, TRUE);   /* enabel keys like F1, arrowkeys ... */
    noecho();               /* disable input buffer on screen e.g. escape stuff*/
    curs_set(2);            /* disable cursor blinki blinki */
    //nodelay(stdscr, 1);     /* */
    //srand(time(NULL));
}

void Dispose()
{
    //ncurse release
    refresh();
    endwin();              /* stop ncurses mode IMPORTANT! ;D*/
}

void ShowSplashScreen()
{
    ClearTerminal();
  
    int fifth = (int) LINES / 5;
    int startRow = fifth * 2;
    int startColumn = ((int) COLS / 2) - 25;

    mvprintw(startRow, startColumn,     "    ____    ___    _____    ____   ___________  _____");
    mvprintw(startRow + 1, startColumn, "   /  _/ | / / |  / /   |  / __ \\/ ____/ __ \\/ ___/");
    mvprintw(startRow + 2, startColumn, "   / //  |/ /| | / / /| | / / / / __/ / /_/ /\\__ \\ ");
    mvprintw(startRow + 3, startColumn, " _/ // /|  / | |/ / ___ |/ /_/ / /___/ _, _/___/ / ");
    mvprintw(startRow + 4, startColumn, "/___/_/ |_/  |___/_/  |_/_____/_____/_/ |_|/____/  ");
    mvprintw(startRow + 5, startColumn, "lines: %d, cols: %d", LINES, COLS);
    mvprintw(startRow * 3, startColumn + 10, "press any key to start .. ");
    refresh();
    getch();

    /* clear screen */
    ClearTerminal();
}

/* ================================================================================================================= */
/* ====================================================== DRAW ===================================================== */
/* ================================================================================================================= */
void Draw()
{
    /*draw invaders*/
    struct List *invaderList = (struct List *)GetFirstElement(invaders);
    if(invaderList != NULL) 
    {
        while (invaderList->Next != NULL)
        {
            PrintChar(invaderList);

            if (invaderList->Next == NULL) 
                break;

            invaderList = invaderList->Next;
        }
    }
}

void PrintChar(struct List *object)
{
    if (object->Entity->SymbolSwitch)
        mvaddch(object->Entity->Position->Row, object->Entity->Position->Column, object->Entity->SymbolOne);
    else
        mvaddch(object->Entity->Position->Row, object->Entity->Position->Column, object->Entity->SymbolOne);       
        // printf("%c", object->Entity->SymbolTwo);   
}

void DeleteChar(struct Position *pos)
{
    mvaddch(pos->Row, pos->Column, ' ');
}

/* ================================================================================================================= */
/* ====================================================== LOGIC ==================================================== */
/* ================================================================================================================= */
void Initialize()
{
    /* create invaders */
    while ( GetListCount(invaders) < (_InvaderPerRow * _InvaderRowCount) )
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

        listElement->Entity->SymbolOne = _InvaderAppearence;
        listElement->Entity->SymbolTwo = _InvaderAppearenceTwo;
        listElement->Entity->SymbolSwitch = false;

        /* add to List */
        invaders = (struct List *)AddElement(invaders, listElement);
    }

    refresh();
}

void GetNextPosition(struct Position *lastPosition, struct Position *newPosition, int listCount)
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
                invaderList->Entity->Position->Column = invaderList->Entity->Position->Column + _MoveHorizontalStep;
            }
            else
            {
                invaderList->Entity->Position->Column = invaderList->Entity->Position->Column - _MoveHorizontalStep;
            }
        }

        /* draw */
        //PrintChar(invaderList);
        invaderList->Entity->SymbolSwitch = !invaderList->Entity->SymbolSwitch;

        /* next */
        invaderList = invaderList->Next;
    }
    invaderMoveForwart = false;
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
        if (i++ > _InvaderPerRow)
            break;
    }

    if (min == 1 && invaderDirection == true)
    {
        invaderDirection = !invaderDirection;
        bounceCounter++;
    }
    if (max == COLS && invaderDirection == false)
    {
        invaderDirection = !invaderDirection;
        bounceCounter++;
    }
}
