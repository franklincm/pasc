struct StrLinkedList{
  char *str;
  struct StrLinkedList *next;
} StrLinkedList;

typedef struct StrLinkedList *strnode;

strnode createStrNode(char * str);
strnode addNode(strnode head, char * str);
