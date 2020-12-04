#include <stdlib.h>
#include <string.h>
#include "../headers/linkedlist.h"

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

node addNode(node head, char *str) {
  node tmp, p;
  tmp = createNode();
  tmp->str = str;
  if(head == NULL) {
    head = tmp;
  } else {
    p = head;
    while(p->next != NULL) {
      p = p->next;
    }
    p->next = tmp;
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
