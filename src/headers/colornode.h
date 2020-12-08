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

void push_eye(struct ColorNode *GreenNode);
void pop_eye();
void prune_list();
void insert_node(char color, char *lex, int type, char *profile);
int search_green(char *lex);
int search_blue(char *lex);
void check_add_green(Token t);
void check_add_blue(char *lex, int type);
uintptr_t get_tail_address();
uintptr_t get_node_addr(struct ColorNode *n);
struct ColorNode *get_parent_green();
void update_profile(int type);
void set_return_type(int type, node *symbol_table);
char *profile_type_to_str(int type);

