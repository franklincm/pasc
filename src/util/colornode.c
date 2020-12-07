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

void push_green(struct ColorNode *GreenNode) {
  struct StackNode *node = malloc(sizeof(struct StackNode));
  node->addr = (uintptr_t)GreenNode;

  if (eye_stack == NULL){
    eye_stack = node;
    printf("push 0x%" PRIXPTR ": %s\n", node->addr, GreenNode->lex);
    return;
  }

  node->next = eye_stack;
  eye_stack = node;
  printf("push 0x%" PRIXPTR ": %s\n", node->addr, GreenNode->lex);
}

void pop_green() {
  if (eye_stack == NULL) {
    return;
  }

  struct StackNode *tmp;
  tmp = eye_stack;
  
  if (eye_stack->next) {
    eye_stack = eye_stack->next;
    printf("pop: 0x%" PRIXPTR "\n", tmp->addr);
  } else {
    printf("pop: 0x%" PRIXPTR "\n", tmp->addr);
    eye_stack = NULL;
  }
}

/*
  This will push onto 2 stacks: dllist, and eye. dllist
  will keep both green and blue nodes, while eye will keep only green
  nodes.
 */
void insert_green(char *lex, int type, char *profile) {
  struct ColorNode *node = malloc(sizeof(struct ColorNode));
  node->lex = lex;
  node->type = type;
  node->profile = profile;

  push_green(node);

  if (dllist == NULL) {
    dllist = node;
    printf("address of %s: 0x%" PRIXPTR "\n", node->lex, (uintptr_t)node);
    return;
  }

  struct ColorNode *tmp;
  tmp = dllist;
  while(tmp->down) {
    tmp = tmp->down;
  }
  node->up = tmp;
  tmp->down = node;

  printf("address of %s: 0x%" PRIXPTR "\n", node->lex, (uintptr_t)node);
}

void prune_list() {
  pop_green();

  if (dllist == NULL) {
    return;
  } else if (!dllist->down) {
    printf("POP Green : %s\n", dllist->lex);
    printf("address of %s: 0x%" PRIXPTR "\n", dllist->lex, (uintptr_t)dllist);
    dllist = NULL;
    return;
  }
  
  struct ColorNode *tmp;
  tmp = dllist;
  // go to end of list
  while(tmp->down->down) {
    tmp = tmp->down;
  }
  printf("POP Green : %s\n", tmp->down->lex);
  printf("address of %s: 0x%" PRIXPTR "\n", tmp->down->lex, (uintptr_t)tmp->down);
  tmp->down = NULL;
}

/*
  returns whether or not the lexeme is found in the existing
  green nodes. 

  0 = not found
  1 = found
 */
int search_green_nodes(char *lex) {
  //printf("search: %s\n", lex);
  if (dllist == NULL) {
    return 0;
  }
  struct ColorNode *tmp;
  tmp = dllist;
  // strcmp() == 0 when identical
  if (!strcmp(tmp->lex, lex)) {
    printf("found: %s\n", tmp->lex);
    return 1;
  }

  // while list has more children...
  while(tmp->down) {
    tmp = tmp->down;
    if (!strcmp(tmp->lex, lex)) {
      printf("found: %s\n", tmp->lex);
      return 1;
    }
  }
  return 0;
}

void check_add_green_node(Token t) {
  char *str = t.str;
  int type = t.type;
  char *profile = "test";

  /* TODO: check parent green nodes first */
  if(search_green_nodes(str)) {
    printf("SEMERR: Attempt to redefine `%s`\n", str);
    insert_green("SEMERR", type, profile);
    printf("PUSH SEMERR Green : %s\n", str);    
  }

  insert_green(str, type, profile);
  printf("PUSH Green : %s\n", str);
}
