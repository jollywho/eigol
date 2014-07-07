#include "stdio.h"
#include "ncurses.h"
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include "locale.h"
#include "stdlib.h"
#include "uthash.h"

typedef struct braille {
  bool status;
  char bit[8];
} braille;
typedef struct cell {
  bool alive;
  bool status;
  bool bstatus;
  braille* bcell;
} cell;
cell** c_cells;
cell** n_cells;
braille** block;
int col = 0;
int row = 0;
int bcol = 0;
int brow = 0;

struct my_struct {
    char id[10];
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
      if (c_cells[i][j].alive)
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
      if (!c_cells[i][j].alive)
      {
        if (!n_cells[i][j].status)
        {
          if (countblock_dead(i,j) == 2)
          {
            n_cells[i][j].alive = true;
          }
        }
      }
      else
        count++;
    }
  }
  return count-1;
}

void update()
{
  for (int i=0; i<row; i++)
  {
    for (int j=0; j<col; j++)
    {
      if (c_cells[i][j].alive)
      {
        int b = countblock_alive(i,j);
        if (b == 3 || b == 2)
          n_cells[i][j].alive = true;
      }
    }
  }
}

void draw()
{
  for (int i=0; i<brow; i++)
  {
    for (int j=0; j<bcol; j++)
    {
      attron(COLOR_PAIR(2));
      if (block[i][j].status)
        mvprintw(i,j, "%s","x");
        //mvprintw(i,j, "%ls",find_user(block[i/4][j/2].bit)->name);
      attroff(COLOR_PAIR(2));
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

void set_braille_nodes(int r, int c)
{
  int bit = 0;
  fprintf(stderr, "%d,%d\n",r, c);
  for (int i=r; i<r+4; i++)
  {
    for (int j=c; j<c+2; j++)
    {
      if (i >= 0 && j >= 0 &&
          i < row && j < col)
      {
        if (c_cells[i][j].alive)
        {
          fprintf(stderr, "bit %d :%d,%d\n",bit, r, c);
          block[i][j].status = true;
          block[i][j].bit[bit] = '1';
        }
        c_cells[i][j].bstatus = true;
      }
      bit++;
    }
  }
}

void braillify()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      set_braille_nodes(i,j);
    }
  }
}

void copy_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      c_cells[i][j].alive = n_cells[i][j].alive;
      c_cells[i][j].status = n_cells[i][j].status;
    }
  }
}

void reset_cycle()
{
  for (int i=0;i<row;i++)
  {
    for (int j=0;j<col;j++)
    {
      n_cells[i][j].alive = false;
      n_cells[i][j].status = false;
      n_cells[i][j].bstatus = false;
    }
  }
  for (int i=0;i<brow;i++)
  {
    for (int j=0;j<bcol;j++)
    {
      block[i][j].status = false;
      for (int n=0;n<8; n++)
      {
        block[i][j].bit[n] = '0';
      }
    }
  }
  return;
}

void create()
{
  c_cells = (cell**)malloc(row * sizeof(cell*));
  n_cells = (cell**)malloc(row * sizeof(cell*));
  block = (braille**)malloc(brow * sizeof(braille*));
  for (int i=0;i<row;i++)
  {
    c_cells[i] = (cell*)malloc(col * sizeof(cell));
    n_cells[i] = (cell*)malloc(col * sizeof(cell));
  }
  for (int i=0;i<bcol;i++)
    block[i] = (braille*)malloc(bcol * sizeof(braille));
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
  reset_cycle();

  //for (int i=0;i<row;i++)
  //  c_cells[i][0].alive = true;

  while(1)
  {
    clrscr();
    braillify();
    draw();
    update();
    refresh();
    copy_cycle();
    reset_cycle();
    fprintf(stderr, "==============\n");
  }
  for (int i=0;i<row;i++)
  {
    free(c_cells[i]);
    free(c_cells[i]);
  }

  clear();
  endwin();
  return 0;
}
