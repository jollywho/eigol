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
  bool brailled;
} cell;
typedef struct block {
  bool status;
  char bit[9];
} block;
block** blocks;
cell** ccells;
cell** ncells;
int col = 0;
int row = 0;
int brow = 0;
int bcol = 0;

struct my_struct {
    char* id;
    wchar_t name;
    UT_hash_handle hh;
};

struct my_struct *users = NULL;

void add_user(char* key, wchar_t value) {
    struct my_struct *s;

    s = (struct my_struct*)malloc(sizeof(struct my_struct));
    s->id = strdup(key);
    s->name = value;
    HASH_ADD_KEYPTR(hh, users, s->id, strlen(s->id), s);
}

struct my_struct* find_user(char* key) {
    struct my_struct *s;

    HASH_FIND_STR( users, key, s );
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
      if (ccells[i][j].alive)
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
      if (!ccells[i][j].alive)
      {
        if (!ncells[i][j].status)
        {
          if (countblock_dead(i,j) == 2)
          {
            ncells[i][j].alive = true;
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
  for (int i=0; i<brow; i++)
  {
    for (int j=0; j<bcol; j++)
    {
      mvprintw(i,j, "%lc", find_user(blocks[i][j].bit)->name);
    }
  }
  for (int i=0; i<row; i++)
  {
    for (int j=0; j<col; j++)
    {
      //mvprintw(i,j, "%lc", find_user(blocks[i][j].bit)->name);
      if (ccells[i][j].alive)
      {
        int b = countblock_alive(i,j);
        if (b == 3 || b == 2)
          ncells[i][j].alive = true;
      }
    }
  }
}

void clrscr()
{
  for (int i=0; i<brow; i++)
  {
    for (int j=0; j<bcol; j++)
    {
      mvprintw(i,j, "%s"," ");
    }
  }
}

void set_braille_nodes(int r, int c, int a, int b)
{
  int bit = 0;
  for (int i=r; i<r+4; i++)
    for (int j=c; j<c+2; j++)
  {
    if (i >= 0 && j >= 0 &&
        i < row && j < col)
    {
      if (ccells[i][j].alive)
      {
        blocks[a][b].bit[bit] = '1';
      }
      ccells[i][j].brailled = true;
    }
    bit++;
  }
}

void braillify()
{
  int r = 0;
  int c = 0;
  int a = 0;
  int b = 0;
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
        set_braille_nodes(i,j,r,c);
      if (b > 1)
      {
        b = 0;
        c++;
      }
      b++;
    }
    if ( a > 3)
    {
      a = 0;
      r++;
    }
    a++;
    c = 0;
  }
}

void copy_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      ccells[i][j].alive = ncells[i][j].alive;
      ccells[i][j].status = ncells[i][j].status;
    }
  }
}

void reset_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      ncells[i][j].alive = false;
      ncells[i][j].brailled = false;
      ncells[i][j].status = false;
    }
  }
  for (int i=0;i<brow;i++)
  {
    for (int j=0;j<bcol;j++)
    {
      blocks[i][j].status = false;
      for (int n=0; n<8; n++)
      {
        blocks[i][j].bit[n] = '0';
      }
    }
  }
  return;
}

void create()
{
  ccells = (cell**)malloc(row * sizeof(cell*));
  ncells = (cell**)malloc(row * sizeof(cell*));
  blocks = (block**)malloc(brow * sizeof(block*));
  for (int i=0;i<row;i++)
  {
    ccells[i] = (cell*)malloc(col * sizeof(cell));
    ncells[i] = (cell*)malloc(col * sizeof(cell));
  }
  for (int i=0;i<brow;i++)
    blocks[i] = (block*)malloc(bcol * sizeof(block));
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

  brow = row;
  bcol = col;
  row*=4;
  col*=2;
  fprintf(stderr, "row, col %d, %d\n", row, col);
  gen_table();
  create();

  for (int i=0;i<row/2*col/2;i++)
  {
    int q = rand() % row;
    int f = rand() % col;
    ccells[q][f].alive = true;
  }

  while(1)
  {
    reset_cycle();
    braillify();
    clrscr();
    draw();
    refresh();
    copy_cycle();
    fprintf(stderr, "==============\n");
  }
  for (int i=0;i<row;i++)
  {
    free(ccells[i]);
    free(ccells[i]);
  }

  clear();
  endwin();
  return 0;
}
