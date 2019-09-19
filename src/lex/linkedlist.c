#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/linkedlist.h"

strnode createStrNode(char * str) {
  strnode tmp;
  tmp = (strnode)malloc(sizeof(struct StrLinkedList));
  tmp->str = str;
  tmp->next = NULL;
  return tmp;
}

strnode addNode(strnode head, char * str) {
  strnode tmp, p;
  tmp = createStrNode(str);

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
