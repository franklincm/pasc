#include <stdio.h>

#ifndef RESERVED
#define RESERVED
#include "./reserved.h"
#endif

#ifndef LINKED_LIST
#define LINKED_LIST
#include "./linkedlist.h"
#endif

#define LEXERR 99
#define SYNERR 98
#define TOKEN_WS 0
#define TOKEN_ID 1
#define TOKEN_RELOP 2
#define TOKEN_OP 3
#define TOKEN_ADDOP 4
#define TOKEN_MULOP 5
#define TOKEN_COMMA 6
#define TOKEN_LPAREN 7
#define TOKEN_RPAREN 8
#define TOKEN_ELIPSIS 9
#define TOKEN_DOT 10
#define TOKEN_ASSIGN 11
#define TOKEN_COLON 12
#define TOKEN_SEMICOLON 13
#define TOKEN_LBRACKET 14
#define TOKEN_RBRACKET 15
#define TOKEN_INT 16
#define TOKEN_REAL 17
#define TOKEN_EOF -1

#define TOKEN_PROGRAM 100
#define TOKEN_VAR 101
#define TOKEN_ARRAY 102
#define TOKEN_OF 103
#define TOKEN_INTEGER 104
#define TOKEN_RREAL 105
#define TOKEN_FUNCTION 106
#define TOKEN_PROCEDURE 107
#define TOKEN_BEGIN 108
#define TOKEN_END 109
#define TOKEN_IF 110
#define TOKEN_THEN 111
#define TOKEN_ELSE 112
#define TOKEN_WHILE 113
#define TOKEN_DO 114
#define TOKEN_NOT 115
#define TOKEN_OR 116
#define TOKEN_DIV 117
#define TOKEN_MOD 118
#define TOKEN_AND 119

#define PGPARAM 9
#define p_NULL 0
#define p_INT 1
#define p_REAL 2
#define p_AINT 3
#define p_AREAL 4
#define p_PPINT 5
#define p_PPREAL 6
#define p_PPAINT 7
#define p_PPAREAL 8


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

Token get_token(FILE *input, FILE *listing, FILE *tokenfile, node ReservedWords, node *SymbolTable);
Token get_token_from_line(char *line, node ReservedWords, node *SymbolTable);
Token nfa(char *f, node ReservedWords, node *SymbolTable);
Token dfa_idres(char *f, node ReservedWords, node *SymbolTable);
Token dfa_whitespace(char *f);
Token dfa_int(char *f);
Token dfa_relops(char *f);
Token dfa_catchall(char *f);
Token dfa_long_real(char *f);
Token dfa_real(char *f);
char *type_to_str(int type);
char *attr_to_str(Token t);
