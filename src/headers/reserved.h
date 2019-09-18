int parse_reserved_words();

typedef struct {
  char lexeme[20];
  char type[6];
  int attr;
} Token;
