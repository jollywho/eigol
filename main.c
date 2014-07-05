#include "stdio.h"
#include "ncurses.h"
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include "locale.h"
#include "stdlib.h"
#include "uthash.h"

typedef struct cell {
  bool alive;
  bool status;
  bool root;
  bool brailled;
  char bit[8];
} cell;
cell** c_cycle;
cell** n_cycle;
int col = 0;
int row = 0;

struct my_struct {
    char id[8];
    wchar_t name[50];
    UT_hash_handle hh;
};

struct my_struct *users = NULL;

void add_user(char* user_id, wchar_t* name) {
    struct my_struct *s;

    s = (struct my_struct*)malloc(sizeof(struct my_struct));
    strncpy(s->id, user_id,8);
    wcsncpy(s->name, name,50);
    HASH_ADD_INT( users, id, s );
}

struct my_struct *find_user(char* user_id) {
    struct my_struct *s;

    HASH_FIND_INT( users, user_id, s );
    return s;
}

void gen_table()
{
#include "table.h"
}

int countblock_dead(int r, int c)
{
  int count = 0;
  for (int i=r-1; i<r+2; i++)
    for (int j=c-1; j<c+2; j++)
  {
    if (i >= 0 && j >= 0 &&
        i < row && j < col)
    {
      if (c_cycle[i][j].alive)
        count++;
    }
  }
  return count-1;
}

int countblock_alive(int r, int c)
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
        if (!n_cycle[i][j].status)
        {
          if (countblock_dead(i,j) == 2)
          {
            n_cycle[i][j].alive = true;
          }
        }
      }
      else
        count++;
    }
  }
  return count-1;
}

void draw()
{
  for (int i=0; i<row; i++)
  {
    for (int j=0; j<col; j++)
    {
      if (c_cycle[i][j].alive)
      {
        int b = countblock_alive(i,j);
         // mvprintw(i/4,j/2, "%ls",find_user(c_cycle[i][j].bit)->name);
        attron(COLOR_PAIR(2));
        mvprintw(i/4,j/2, "%s"," ");
        attroff(COLOR_PAIR(2));
        if (b == 3 || b == 2)
          n_cycle[i][j].alive = true;
      }
    }
  }
}

void clrscr()
{
  for (int i=0; i<row; i++)
  {
    for (int j=0; j<col; j++)
    {
      mvprintw(i/4,j/2, "%s"," ");
    }
  }
}

void set_braille_nodes(int r, int c, cell* cj)
{
  int bit = 0;
  c_cycle[r][c].root = true;
  for (int i=r; i<r+2; i++)
    for (int j=c; j<c+4; j++)
  {
    if (i >= 0 && j >= 0 &&
        i < row && j < col)
    {
      if (c_cycle[i][j].alive)
        cj->bit[bit] = '1';
      c_cycle[i][j].brailled = true;
    }
    bit++;
  }
}

void braillify()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      if (!c_cycle[i][j].brailled)
      { 
        set_braille_nodes(i,j, &c_cycle[i][j]);
      }
    }
  }
}

void copy_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      c_cycle[i][j].alive = n_cycle[i][j].alive;
      c_cycle[i][j].status = n_cycle[i][j].status;
    }
  }
}

void reset_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      n_cycle[i][j].alive = false;
      n_cycle[i][j].root = false;
      n_cycle[i][j].brailled = false;
      n_cycle[i][j].status = false;
      for (int n=0; n<8; n++)
      {
        n_cycle[i][j].bit[n] = '0';
        c_cycle[i][j].bit[n] = '0';
      }
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

//todo:
//braille groupings means there can be many more cells
//col can extend x2 and row x4
//0-40 & 0-80
//0-80 & 0-3207
//i,j becomes |i/2|, |j/4|
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

  row*=4;
  col*=2;
  fprintf(stderr, "row, col %d, %d\n", row, col);
  gen_table();
  create();
  reset_cycle();
  for (int i=0;i<row/2*col/2;i++)
  {
    int q = rand() % row;
    int f = rand() % col;
    c_cycle[q][f].alive = true;
  }
  for (int i=0;i<row;i++)
    c_cycle[i][i].alive = true;

  while(1)
  {
    clock_t start = clock(), diff;
    braillify();
    clrscr();
    draw();
    diff = clock() - start;
    refresh();
    copy_cycle();
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
