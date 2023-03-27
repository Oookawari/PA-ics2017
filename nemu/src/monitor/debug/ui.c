#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
  if(free_ == NULL) {
    printf("No free watchpoint!");
    assert(0);
    return NULL;
  }
  else {
    WP* given = free_;
    free_ = given->next;
    given->next = head;
    head = given;
    return given;
  }
}

void free_wp(WP *wp) {
  WP* temp = head;
  if(head == wp) {
    head = wp->next;
    wp->next = free_;
    free_ = wp;
    return;
  }
  while(temp->next != wp)
    temp = temp->next;
  temp->next = wp->next;
  wp->next = free_;
  free_ = wp;
  return;
}

WP* find_wp(int no) {
  WP* temp = head;
  while(temp != NULL) {
    if(temp->NO == no) return temp;
    temp = temp->next;
  }
  return NULL;
}

void show_wps(){
  WP* temp = head;
    printf("-----------------------\n");
  while(temp != NULL) {
    int num = temp->NO;
    printf("No: %d\t",num);
    printf("Expr: %s\t\t\t",temp->expr);
    printf("Value: %d\n",temp->init_value);
    temp = temp ->next;
  }
    printf("-----------------------\n");
}

bool check_watchpoint(){
  WP* temp = head;
  bool res = true;
  while(temp != NULL) {
    bool success;
    int expr_value = expr(temp->expr, &success);
    if(success == false || expr_value != temp->init_value){
      printf("watchpoint - changed:\n");
      int num = temp->NO;
      printf("No: %d\t",num);
      printf("Expr: %s\t",temp->expr);
      printf("ORI_Value: %d\t",temp->init_value);
      printf("CUR_Value: %d\n",expr_value);
      res = false;
      temp->init_value = expr_value;
    }
    temp = temp ->next;
  }
  return res;
}