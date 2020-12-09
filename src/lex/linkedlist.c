#include <stdlib.h>
#include <string.h>

#ifndef _LINKEDLIST
#define _LINKEDLIST
#include "../headers/linkedlist.h"
#endif

node createNode() {
  node tmp;
  tmp = (node)malloc(sizeof(struct LinkedList));
  tmp->next = NULL;
  return tmp;
}

node insertNode(node head, node n) {
  node p;
  if(head == NULL) {
    head = n;
  } else {
    p = head;
    while(p->next != NULL) {
      p = p->next;
    }
    p->next = n;
  }
  return head;
}

node getNode(node head, char *str) {
  while(head != NULL) {
    if(strcmp(head->str, str) == 0) {
      break;
    }
    head = head->next;
  }
  return head;
}

int getType(node head, char *lex) {
  node p = getNode(head, lex);
  return p->type;
}
