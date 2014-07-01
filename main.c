#include "stdio.h"
#include "ncurses.h"
#include <unistd.h>
#include <time.h>
#include "locale.h"

#define DELAY 30000
int ch;
char str[3] = "░";
char str2[3] = "▒";
int col = 0;
int row = 0;

void draw(int q)
{
  clear();
  int x,y = 0;
  for (int c=0; c<row; c++)
  {
    x=0;
    for (int r=0; r<col; r++)
    {
      if (q == -1)
      {
         attron(COLOR_PAIR(1));
         mvprintw(y,x, "%s", str2);

      }
      if (q == 1)
      {
         attron(COLOR_PAIR(2));
        mvprintw(y,x, "%s", str);
      }
      x++;
    }
    y++;
  }
}

int main(int argc, char** argv)
{
  setlocale(LC_ALL, "");
  initscr();
  getmaxyx(stdscr, row, col);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, COLOR_BLACK, COLOR_RED);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  int q = 1;
  while(1)
  {
    clock_t start = clock(), diff;
    draw(q);
    diff = clock() - start;
    fprintf(stderr, "%ju\n", diff);
    if (diff < 500000)
    usleep(500000);
    refresh();
    q *= -1;
  }
  clear();
  endwin();
  return 0;
}
