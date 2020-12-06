#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

struct ColorNode{
  char *lex;
  int type;
  char *profile;
  struct ColorNode *next;
};

void push_green();
void pop_green();
int search_green_nodes(char *lex);
void check_add_green_node(Token t);
