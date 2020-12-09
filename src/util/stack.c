#include <stdlib.h>
#include "../headers/stack.h"

static struct Stack *stack;

void push(char *str) {
  struct Stack *node = malloc(sizeof(struct Stack));
  node->lex = str;

  if (stack == NULL) {
    stack = node;
    return;
  }

  node->next = stack;
  stack = node;
}

struct Stack *pop() {
  struct Stack *p = NULL;
  if (stack == NULL)
    return p;

  p = stack;
  if(stack->next) {
    stack = stack->next;
    return p;
  } else {
    stack = NULL;
    return p;
  }
}
