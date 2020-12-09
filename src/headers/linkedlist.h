typedef struct LinkedList{
  char *str;
  char *profile;
  int type;
  int attr;
  struct LinkedList *next;
} LinkedList;

typedef struct LinkedList *node;

node createNode();
node insertNode(node head, node n);
node addNode(node head, char *str);
node getNode(node head, char *str);
int getType(node head, char *lex);
