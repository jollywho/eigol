#define _GNU_SOURCE
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

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

static block** blocks;
static cell** ccells;
static cell** ncells;
static int col = 0;
static int row = 0;
static int brow = 0;
static int bcol = 0;

typedef struct {
  char* id;
  wchar_t name;
  UT_hash_handle hh;
} braille_cell;

static braille_cell *braille_set = NULL;

braille_cell* find_user(char* key)
{
  braille_cell *bc;
  HASH_FIND_STR(braille_set, key, bc);
  return bc;
}

void add_braille(char* key, wchar_t value)
{
  braille_cell *bc = malloc(sizeof(braille_cell));
  bc->id = strdup(key);
  bc->name = value;
  HASH_ADD_STR(braille_set, id, bc);
}

void gen_table()
{
#include "table.h"
}

int countblock_dead(int r, int c)
{
  int count = 0;
  for (int i=r-1; i<r+2; i++) {
    for (int j=c-1; j<c+2; j++) {
      if (i >= 0 && j >= 0 && i < row && j < col) {
        if (ccells[i][j].alive)
          count++;
      }
    }
  }
  return count-1;
}

int countblock_alive(int r, int c)
{
  int count = 0;
  for (int i=r-1; i<r+2; i++) {
    for (int j=c-1; j<c+2; j++) {
      if (i >= 0 && j >= 0 && i < row && j < col) {
        if (!ccells[i][j].alive) {
          if (!ncells[i][j].status) {
            if (countblock_dead(i,j) == 2) {
              ncells[i][j].alive = true;
            }
          }
        }
        else
          count++;
      }
    }
  }
  return count-1;
}

void draw()
{
  for (int i=0; i<brow; i++) {
    for (int j=0; j<bcol; j++)
      mvprintw(i,j, "%lc", find_user(blocks[i][j].bit)->name);
  }

  for (int i=0; i<row; i++) {
    for (int j=0; j<col; j++) {
      if (ccells[i][j].alive) {
        int b = countblock_alive(i,j);
        if (b == 3 || b == 2)
          ncells[i][j].alive = true;
      }
    }
  }
}

void set_braille_nodes(int r, int c, int a, int b)
{
  int bit = 0;
  for (int i=r; i<r+4; i++) {
    for (int j=c; j<c+2; j++) {
      if (i >= 0 && j >= 0 && i < row && j < col) {
        if (ccells[i][j].alive)
          blocks[a][b].bit[bit] = '1';

        ccells[i][j].brailled = true;
      }
      bit++;
    }
  }
}

void braillify()
{
  int r = 0;
  int c = 0;
  int a = 0;
  int b = 0;
  for (int i=0;i<row;i++) {
    for (int j=0;j<col;j++) {
      set_braille_nodes(i,j,r,c);
      if (b > 1) {
        b = 0;
        c++;
      }
      b++;
    }
    if ( a > 3) {
      a = 0;
      r++;
    }
    a++;
    c = 0;
  }
}

void copy_cycle()
{
  for (int i=0;i<row;i++) {
    for (int j=0;j<col;j++) {
      ccells[i][j].alive = ncells[i][j].alive;
      ccells[i][j].status = ncells[i][j].status;
    }
  }
}

void reset_cycle()
{
  for (int i=0;i<row;i++) {
    for (int j=0;j<col;j++) {
      ncells[i][j].alive = false;
      ncells[i][j].brailled = false;
      ncells[i][j].status = false;
    }
  }

  for (int i=0;i<brow;i++) {
    for (int j=0;j<bcol;j++) {
      blocks[i][j].status = false;
      for (int n=0; n<8; n++) {
        blocks[i][j].bit[n] = '0';
      }
    }
  }
}

void create()
{
  ccells = (cell**)malloc(row * sizeof(cell*));
  ncells = (cell**)malloc(row * sizeof(cell*));
  blocks = (block**)malloc(brow * sizeof(block*));
  for (int i=0;i<row;i++) {
    ccells[i] = (cell*)malloc(col * sizeof(cell));
    ncells[i] = (cell*)malloc(col * sizeof(cell));
  }
  for (int i=0;i<brow;i++)
    blocks[i] = (block*)malloc(bcol * sizeof(block));
}

int main(int argc, char** argv)
{
  setlocale(LC_ALL, "");
  initscr();
  getmaxyx(stdscr, row, col);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, COLOR_MAGENTA, -1);
  init_pair(2, COLOR_GREEN, -1);
  attron(COLOR_PAIR(1));
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  brow = row;
  bcol = col;
  row*=4;
  col*=2;
  gen_table();
  create();

  for (int c=0;c<col*200;c++) {
    int y = rand() % row;
    int x = rand() % col;
    ccells[y][x].alive = true;
  }

  while(1) {
    reset_cycle();
    braillify();
    draw();
    refresh();
    copy_cycle();
  }
  for (int i=0;i<row;i++) {
    free(ccells[i]);
    free(ccells[i]);
  }

  clear();
  endwin();
  return 0;
}
