struct LinkedList{
  char *str;
  char *type;
  int attr;
  struct LinkedList *next;
} LinkedList;

typedef struct LinkedList *node;

node createNode();
node insertNode(node head, node n);
node addNode(node head, char * str);
