
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "../headers/colornode.h"

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

static struct ColorNode *dllist = NULL;
static struct StackNode *eye_stack = NULL;

/* push green node onto stack */
void push_eye(struct ColorNode *greenNode) {
  struct StackNode *node = malloc(sizeof(struct StackNode));
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
void insert_node(char color, char *lex, int type, char *profile) {
  // creat node to be inserted with given params
  struct ColorNode *node = malloc(sizeof(struct ColorNode));
  node->color = color;
  node->lex = lex;
  node->type = type;
  node->profile = profile;

  // if green node, push onto stack
  if (color == 'G') {
    push_eye(node);
  }

  // if linked list empty, set this node as the head
  if (dllist == NULL) {
    dllist = node;
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
void check_add_green(char *lex, int type, char *profile) {

  if(search_global(lex)) {
    printf("SEMERR: Attempt to redefine `%s`.\n", lex);
    //insert_node('G', "SEMERR", type, profile);
  } else
    insert_node('G', lex, type, profile);
}

/* check for node in local scope with given lex,
   insert blue node if not found.
 */
void check_add_blue(char *lex, int type) {
  if(search_local(lex)) {
    printf("SEMERR: `%s` already defined in this scope.\n", lex);
    return;
  }
  insert_node('B', lex, type, "");
  return;
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
