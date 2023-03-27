#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[100];
  int init_value;
} WP;
WP* new_wp();
void free_wp(WP *wp);
WP* find_wp(int no);
void show_wps();
bool check_watchpoint();
#endif