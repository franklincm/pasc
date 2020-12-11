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
  char *lex;
  struct StackNode *next;
};

static struct ColorNode *dllist;
static struct StackNode *eye_stack;

void push_eye(struct ColorNode *GreenNode);
void pop_eye();
void prune_list();
void insert_node(char color, char *lex, int type, char *profile);
int search_global(char *lex);
int search_local(char *lex);
void check_add_green(char *lex, int type, char *profile);
void check_add_blue(char *lex, int type);
struct ColorNode *get_color_node(char *lex);
char *profile_type_to_str(int type);

