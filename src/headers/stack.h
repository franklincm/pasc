
struct Stack {
  char *lex;
  struct Stack *next;
};

void push(char *str);
struct Stack *pop();
