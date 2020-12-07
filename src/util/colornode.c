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
  printf("search: %s\n", lex);
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
  }

  insert_green(str, type, profile);
  printf("PUSH Green : %s\n", str);
}
