#ifndef RESERVED
#define RESERVED

#include "./reserved.h"

#endif

#ifndef LINKED_LIST
#define LINKED_LIST

#include "./linkedlist.h"

#endif

#define LEXERR 99
#define TOKEN_WS 0
#define TOKEN_ID 1
#define TOKEN_RELOP 2
#define TOKEN_OP 3
#define TOKEN_COMMA 4
#define TOKEN_LPAREN 5
#define TOKEN_RPAREN 6
#define TOKEN_ELIPSIS 7
#define TOKEN_DOT 8
#define TOKEN_ASSIGN 9
#define TOKEN_COLON 10
#define TOKEN_SEMICOLON 11
#define TOKEN_LBRACKET 12
#define TOKEN_RBRACKET 13
#define TOKEN_INT 14
#define TOKEN_REAL 15
#define TOKEN_EOF -1


#define TOKEN_UNRECOGNIZED_SYMBOL 999

/* LEXERR ATTRIBUTES */
#define UNK_SYMBOL -1
#define IDTOOLONG 10
#define INTTOOLONG 20
#define LEADINGZERO 21
#define DIGITTOOLONG 22
#define FRACTOOLONG 23
#define TRAILINGZERO 24
#define EXPONENTTOOLONG 25
#define MISSINGEXPONENT 26

typedef struct Token{
  char *str;
  int type;
  char *f;
  int attr;
} Token;

Token get_token(char * line, node ReservedWords, node *SymbolTable);
Token machine(char * f, node ReservedWords, node *SymbolTable);
Token m_idres(char *f, node ReservedWords, node *SymbolTable);
Token m_whitespace(char *f);
Token m_int(char *f);
Token m_relops(char *f);
Token m_catchall(char *f);
Token m_real(char *f);
Token m_real2(char *f);
char *type_to_str(Token t);
char *attr_to_str(Token t);
