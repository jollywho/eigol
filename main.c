#include "stdio.h"
#include "ncurses.h"
#include <unistd.h>
#include <time.h>
#include "locale.h"
#include "stdlib.h"

typedef struct cell {
  bool alive;
  bool status;
} cell;
cell** c_cycle;
cell** n_cycle;
int ch;
char str[3] = "░";
char str2[3] = "▒";
int col = 0;
int row = 0;

int countblock(int r, int c, bool top)
{
  int count = 0;
  for (int i=r-1; i<r+2; i++)
    for (int j=c-1; j<c+2; j++)
  {
    if (i >= 0 && j >= 0 &&
        i < row && j < col)
    {
      if (!c_cycle[i][j].alive)
      {
        if (!n_cycle[i][j].status && top)
        {
          n_cycle[i][j].status = true;
          attron(COLOR_PAIR(2));
          mvprintw(i,j, "%s", "$");
          if (countblock(i,j,false) == 3)
          {
            mvprintw(i,j, "%s", "@");
            n_cycle[i][j].alive = true;
          }
        }
      }
      else
        count++;
    }
  }
  return count;
}

void draw()
{
  for (int i=0; i<row; i++)
  {
    for (int j=0; j<col; j++)
    {
      if (c_cycle[i][j].alive)
        {
          int b = countblock(i,j,true);
          mvprintw(i,j, "%s", "x");
          if (b == 2 || b == 3)
            n_cycle[i][j].alive = true;
      }
    }
  }
}

void generation(cell** old, cell** new)
{
  cell* temp = *old;
  *new = *old;
  *old = temp;
}

void reset_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      n_cycle[i][j].alive = false;
      n_cycle[i][j].status = false;
    }
  }
  return;
}

void create()
{
  c_cycle = (cell**)malloc(row * sizeof(cell*));
  n_cycle = (cell**)malloc(row * sizeof(cell*));
  for (int i=0;i<row;i++)
  {
    c_cycle[i] = (cell*)malloc(col * sizeof(cell));
    n_cycle[i] = (cell*)malloc(col * sizeof(cell));
  }
  return;
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
  init_pair(2, COLOR_BLACK, COLOR_CYAN);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  fprintf(stderr, "row, col %d, %d\n", row, col);
  create();
  c_cycle[35][33].alive = true;
  c_cycle[35][34].alive = true;
  c_cycle[35][35].alive = true;
  while(1)
  {
    clock_t start = clock(), diff;
    draw();
    diff = clock() - start;
    if (diff < 600000)
      usleep(600000);
    refresh();
    generation(n_cycle, c_cycle);
    reset_cycle();
    fprintf(stderr, "==============\n");
  }
  for (int i=0;i<row;i++)
  {
    free(c_cycle[i]);
    free(c_cycle[i]);
  }

  clear();
  endwin();
  return 0;
}
