#ifndef LINKED_LIST
#define LINKED_LIST

#include "./linkedlist.h"

#endif

typedef struct ReservedWord{
  char *str;
  char *type;
  int attr;
} ReservedWord;


node parse_reserved_words();
