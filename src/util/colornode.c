
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "../headers/colornode.h"

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

#ifndef OUTPUT
#define OUTPUT
#include "../headers/output.h"
#endif

struct ColorNode *dllist = NULL;
struct StackNode *eye_stack = NULL;
int print_header = 1;

/* push green node onto stack */
void push_eye(struct ColorNode *greenNode) {
  struct StackNode *node = (struct StackNode *)malloc(sizeof(struct StackNode));
  node->addr = (uintptr_t)greenNode;
  node->lex = greenNode->lex;

  if (eye_stack == NULL){
    eye_stack = node;
    return;
  }

  node->next = eye_stack;
  eye_stack = node;
}

/* prunes the linked list up to the current green node
   and then pops the green node off the stack */
void pop_eye() {
  if (eye_stack == NULL) {
    return;
  }

  struct StackNode *tmp;
  tmp = eye_stack;

  if (eye_stack && dllist) {
    while((uintptr_t)get_tail() != eye_stack->addr) {
      prune_list();
    }
  }

  if (eye_stack->next) {
    eye_stack = eye_stack->next;
  } else {
    eye_stack = NULL;
  }
}

/* removes last item in linked list */
void prune_list() {

  if (dllist == NULL) {
    return;
  } else if (!dllist->down) {
    dllist = NULL;
    return;
  }
  
  struct ColorNode *tmp;
  tmp = dllist;
  // go to end of list
  while(tmp->down->down) {
    tmp = tmp->down;
  }

  tmp->down = NULL;
}

/*
  This will push onto 2 stacks: dllist, and eye. dllist
  will keep both green and blue nodes, while eye will keep only green
  nodes.
 */
void insert_node(char color, char *lex, int type, char *profile, int attr, int offset, FILE *sym_table_file) {
  // creat node to be inserted with given params
  struct ColorNode *node = (struct ColorNode *)malloc(sizeof(struct ColorNode));
  node->color = color;
  node->lex = lex;
  node->type = type;
  node->profile = profile;
  node->attr = attr;
  node->offset = offset;

  // if green node, push onto stack
  if (color == 'G') {
    push_eye(node);
  }

  // if linked list empty, set this node as the head
  if (dllist == NULL) {
    dllist = node;
    write_line_to_symtable(color, node->attr, node->offset, node->lex, node->type, node->profile, sym_table_file);
    return;
  }

  // traverse down to the bottom of the linked list
  struct ColorNode *tmp;
  tmp = dllist;
  while(tmp->down) {
    tmp = tmp->down;
  }

  // set created node's up pointer to previous bottom of list
  node->up = tmp;

  // add created node to bottom of list
  tmp->down = node;
  write_line_to_symtable(color, node->attr, node->offset, node->lex, node->type, node->profile, sym_table_file);
}

/*
  searches entire linked list for lex, bottom-up
  0 = not found
  1 = found
 */
int search_global(char *lex) {
  if (dllist == NULL) {
    return 0;
  }
  struct ColorNode *tmp;
  tmp = dllist;

  // traverse to bottom of list...
  while(tmp->down)
    tmp = tmp->down;

  // beginning at bottom of list
  // check node for matching lex
  // and continue moving up
  if(!strcmp(tmp->lex, lex))
    return 1;
  else {
    while(tmp->up) {
      tmp = tmp->up;
      if (!strcmp(tmp->lex, lex)) {
        return 1;
      }
    }
  }
  return 0;
}

/*
  searches linked list for lex, bottom-up
  until reaching first green node
  0 = not found
  1 = found
 */
int search_local(char *lex) {
  if (!dllist) {
    return 0;
  }

  struct ColorNode *tmp;
  tmp = dllist;

  // travers to bottom of list
  while(tmp->down)
    tmp = tmp->down;

  // while not a green node and can still move up
  // the list, check for lex and move up if not found
  while(tmp->color != 'G' && tmp->up) {
    if(!strcmp(tmp->lex, lex))
      return 1;
    tmp = tmp->up;
  }
  return 0;
}

/* check for node in current scope with given lex,
   insert green node if not found.
 */
int check_add_green(char *lex, int type, char *profile, int attr, int offset, FILE *symboltable) {
  //char lex_buf[10];
  //sprintf(lex_buf, "d%d", (rand() % 20));
  
  if(search_global(lex)) {
    //insert_node('G', lex_buf, 99, profile, attr, offset, symboltable);
    return 0;
  } else {
    insert_node('G', lex, type, profile, attr, offset, symboltable);
    return 1;
  }
}

/* check for node in local scope with given lex,
   insert blue node if not found.
 */
int check_add_blue(char *lex, int type, int attr, int offset, FILE *symboltable) {
  if(search_local(lex)) {
    return 0;
  }
  insert_node('B', lex, type, "", attr, offset, symboltable);
  return 1;
}

/* same as global_search but returns the node if found */
struct ColorNode *get_color_node(char *lex) {
  if (dllist == NULL) 
    return NULL;

  struct ColorNode *tmp;
  tmp = dllist;

  // traverse to bottom of list
  while(tmp->down)
    tmp = tmp->down;

  // beginning at bottom of list
  // check node for matching lex
  // and continue moving up
  if(!strcmp(tmp->lex, lex))
    return tmp;
  else {
    while(tmp->up) {
      tmp = tmp->up;
      if (!strcmp(tmp->lex, lex)) {
        return tmp;
      }
    }
  }
  return NULL;
}

/* returns last node in the list */
struct ColorNode *get_tail() {
  if(dllist == NULL)
    return NULL;

  struct ColorNode *tmp;
  tmp = dllist;

  // traverse to end of list and return node
  while(tmp->down)
    tmp = tmp->down;
  return tmp;
}

char *profile_type_to_str(int type) {
  char *str;
  switch(type) {
  case 1:
    str = "INT";
    break;
  case 2:
    str = "REAL";
    break;
  case 3:
    str = "AINT";
    break;
  case 4:
    str = "AREAL";
    break;
  case 5:
    str = "PPINT";
    break;
  case 6:
    str = "PPREAL";
    break;
  case 7:
    str = "PPAINT";
    break;
  case 8:
    str = "PPAREAL";
    break;
  case 9:
    str = "PGPARAM";
    break;
  case 10:
    str = "BOOL";
    break;
  case 51:
    str = "ERROR";
    break;
  }
  return str;
}

void write_line_to_symtable(char color, int attr, int offset, char *lex, int type, char *profile, FILE *sym_table_file) {

  if(color == 'G')
    return;
  
  char line_buffer [100];

  if (print_header) {
    sprintf(line_buffer, "%8s%8s%8s%6s\n", "address", "offset", "lexeme", "type");
    write_line_to_file(line_buffer, sym_table_file);
    print_header = 0;
  }

  if (type == t_PPINT || type == t_PPREAL || type == t_PPAINT || type == t_PPAREAL || type == PGPARAM || type == PGNAME ) {
    //sprintf(line_buffer, "%23s%8s%8s\n", lex, profile, profile_type_to_str(type));
    return;
  } else {
    sprintf(line_buffer, "%5d%8d%10s%6d\n", attr, offset, lex, type);
    
  }

  write_line_to_file(line_buffer, sym_table_file);
}
