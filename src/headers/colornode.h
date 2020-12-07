#include <inttypes.h>

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

struct ColorNode {
  char color;
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

static struct ColorNode *dllist;
static struct StackNode *eye_stack;

void insert_node(char color, char *lex, int type, char *profile);
void prune_list();
int search_green_nodes(char *lex);
int search_blue(char *lex);
void push_green(struct ColorNode *GreenNode);
void pop_green();
void check_add_green_node(Token t);
void check_add_blue(char *lex, int type);
uintptr_t get_tail_address();
char *profile_type_to_str(int type);
uintptr_t get_node_addr(struct ColorNode *n);
