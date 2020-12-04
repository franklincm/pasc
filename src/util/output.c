#include <stdlib.h>
#include "../headers/token.h"

char * listing_format = "%-10d%s";
char * listing_synerr_format = "          %s\n";
char * lexerr_format = "%-10s%-30s%s\n";
char * token_head_format = "%-9s%-14s%-25s%s\n";

char *token_heading() {
  char *heading = (char *)malloc(100 * sizeof(char));
  sprintf(heading, token_head_format,
          "Line No.",
          "Lexeme",
          "TOKEN-TYPE",
          "ATTRIBUTE");
  
  return heading;
}

char *token_str(Token t, int lineno) {
  char *line_buffer = (char *)malloc(150 * sizeof(char));
  
  char * type = (char *)malloc(50 * sizeof(char));
  sprintf(type, "(%s)", type_to_str(t.type));

  if(t.type != TOKEN_WS) {
    char *attr_str;
    char *attr;
    if (t.type == TOKEN_ID) {
      attr_str = "ptr to sym tab";
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "loc%d", t.attr);
      
    } else if (t.type == TOKEN_INT) {
      attr_str = "int value";
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "%d", t.attr);
      
    } else {
      attr_str = (char *)malloc(3 * sizeof(char));
      sprintf(attr_str, "%s", attr_to_str(t));
      attr = (char *)malloc(10 * sizeof(char));
      sprintf(attr, "%d", t.attr);
    }
    sprintf(line_buffer, "%4d     %-14s%-4d%-20s %6s (%s)\n", lineno, t.str, t.type, type, attr, attr_str);
  }
  return line_buffer;
}

char *lexerr_str(Token t) {
  char *buffer = (char *)malloc(150 * sizeof(char));
  sprintf(buffer, lexerr_format, type_to_str(t.type), attr_to_str(t), t.str);
  return buffer;
}

char *listing_str(int lineno, char *line) {
  char *buffer = (char *)malloc(150 * sizeof(char));
  sprintf(buffer, listing_format, lineno, line);
  return buffer;
}

char *listing_err(char *line) {
  char *buffer = (char *)malloc(150 * sizeof(char));
  sprintf(buffer, listing_synerr_format, line);
  return buffer;
}

void write_line_to_file(char *line, FILE *fp) {
  fprintf(fp, "%s", line);
}

char *hr_str(int length, char c) {
  char *buffer = (char *)malloc(80 * sizeof(char));
  char *ptr = &buffer[0];
  for(int i = 0; i < length; i++) {
    *ptr = c;
    ptr++;
  }
  *ptr = '\n';
  return buffer;
}
