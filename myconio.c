#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

static struct termios old, new;

/* Init new terminal i/o */
void initTermios(int echo)
{
  tcgetattr(0, &old); //old settings
  new = old; //copy old Settings
  new.c_lflag &= ~ICANON; //disable buffer
  if(echo)
  {
    new.c_lflag |= ECHO; //Add echo flag
  }
  else
  {
    new.c_lflag &= ~ECHO; //test => cflag & echo
  }
  tcsetattr(0, TCSANOW, &new); //Set new terminal settings
}

void resetTermios(void)
{
  tcsetattr(0,TCSANOW, &old); //Restore old settings
}

char getch_(int echo) //private, "overload"
{
  char ch;
  initTermios(echo); //new term settings to skip RETURN need
  ch = getchar();
  resetTermios();
  return ch;
}

char getch(void)
{
  return getch_(0);
}
char getche(void)
{
  return getch_(1);
}