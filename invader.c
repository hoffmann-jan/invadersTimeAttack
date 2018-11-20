/* ?sth. for terminal size access */
#include <sys/ioctl.h>
/* add standard input / output */
#include <stdio.h>
/* add bool */ 
#include <stdbool.h>

void MoveInvaders();
void MoveProjectiles();
void MovePlayer();
void RemoveDefeatedEntities();

int main (void)
{
  /* fields */
  int _Score = 0;
  int _Health = 3;

  /* get terminal size */
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  printf ("lines %d\n", w.ws_row);
  printf ("columns %d\n", w.ws_col);
  /* end terminal size */

  /* game loop */
  while (true)
  {

  }
  /* end game loop */
  
  return 0;
}
