#include <inttypes.h>

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

struct ColorNode {
  char *lex;
  int type;
  char *profile;
  struct ColorNode *up;
  struct ColorNode *down;
};

struct StackNode {
  uintptr_t addr;
  struct StackNode *next;
};

void insert_green();
void prune_list();
int search_green_nodes(char *lex);
void push_green(struct ColorNode *GreenNode);
void pop_green();
void check_add_green_node(Token t);
