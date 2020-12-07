#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

struct ColorNode{
  char *lex;
  int type;
  char *profile;
  struct ColorNode *up;
  struct ColorNode *down;
};

void insert_green();
void prune_list();
int search_green_nodes(char *lex);
void check_add_green_node(Token t);
