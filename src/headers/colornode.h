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
  int attr;
  int offset;
  struct ColorNode *up;
  struct ColorNode *down;
};

struct StackNode {
  uintptr_t addr;
  char *lex;
  struct StackNode *next;
};

void push_eye(struct ColorNode *GreenNode);
void pop_eye();
void prune_list();
void insert_node(char color, char *lex, int type, char *profile, int attr, int offset, FILE *symboltable);
int search_global(char *lex);
int search_local(char *lex);
int check_add_green(char *lex, int type, char *profile, int attr, int offset, FILE *symboltable);
int check_add_blue(char *lex, int type, int attr, int offset, FILE *symboltable);
struct StackNode *get_parent();
struct ColorNode *get_color_node(char *lex);
struct ColorNode *get_tail();
char *profile_type_to_str(int type);
void write_line_to_symtable(char color, int attr, int offset, char *lex, int type, FILE *sym_table_file);
