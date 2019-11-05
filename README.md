Chase Franklin
chase-franklin@utulsa.edu

CS4013 Journal

# 09/01/2019

## created project structure, init git repository
   Setup project structure and started version control

## created Makefile
   Really just a place holder for now. Compiling everything manually with a
   clang command alias.

## readline
   Writing a readline function. Appears to be working and adding line numbers
   as needed for a listing file, but fgets() doesn't seem to grab all 72
   characters from a line unless I give it a buffer of size 79. Been a while
   since I've done anything in C, so I'm going to have to read some docs. I
   wonder if this is even critical given the line length restriction.

# 09/03/2019

## More adventures with Makefile and project structure

   Spent a few hours wrestling with headers and the Makefile so that I can 
   HOPEFULLY keep this somewhat organized as it grows. I think I'm just about
   ready to start writing the "machines" for this and creating a reserved word
   file. I don't expect this to be terribly difficult now that I have some
   semblance of organization in place.

   This will likely change a lot but the current structure looks like this:
   .
   ├── bin
   ├── data
   │   └── example.pas
   ├── Makefile
   ├── notes.txt
   ├── obj
   └── src
       ├── headers
       │   └── relops.h
       ├── lex
       │   ├── identifier.c
       │   ├── int.c
       │   ├── longreal.c
       │   ├── real.c
       │   ├── relops.c
       │   ├── reserved.c
       │   └── ws.c
       └── readline.c


# 09/12/19

  renamed a bunch of stuff. now getting the `example.pas` file from argv.

# 09/16/19

  created reserved word file at `data/reserved` which is loaded in `reserved.h`.
  so far this seems to be parsing the file correctly from within my parsing
  function in `reserved.h`


# 09/17/19

  populating an array with a `reserved word` struct. for now I'm just statically
  allocating an array and passing a pointer to the parsing function.

  typedef struct ReservedWord{
  char *str;
  char *type;
  int attr;
  } ReservedWord;

# 09/17/19

  managed to get a linkedlist struct working. I intend to use this to hold
  identifiers returned by `get_token()`. also had a bunch of unnecessary
  include statements sprinkled throughout my headers - left over from
  printf "debugging" haha - so I removed those.

  My Makefile is getting uglier by the minute.

# 09/18/19, 09/19/19, 09/20/19

  I ran into several hours of trouble with the Makefile trying to get it
  to compile everything correctly. Turns out `gcc` doesn't like when
  `main.c` and dependent header files include the same user defined
  headers.

  I've finally managed to start parsing with one machine - idres. I'm passing
  in a single line of source, and then using a pointer to advance through the
  string, at which point I create a substring from the line in a buffer, then
  store that substring and the pointer in a `Token` struct that gets returned.

  After some reorganization the new directory structure looks like this:
  
```
  ├── bin
  │   └── lex
  ├── data
  │   ├── example.pas
  │   └── reserved
  ├── Makefile
  ├── notes.txt
  ├── obj
  │   ├── linkedlist.o
  │   ├── main.o
  │   ├── reserved.o
  │   └── token.o
  └── src
      ├── headers
      │   ├── linkedlist.h
      │   ├── reserved.h
      │   └── token.h
      ├── lex
      │   ├── linkedlist.c
      │   ├── reserved.c
      │   └── token.c
      └── main.c
```

Makefile
```
  CC = clang
  CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
  DEPS = reserved.h linkedlist.h

  VPATH = src:src/lex/:obj
  vpath %.h src/headers/
  vpath %.o obj

  obj/%.o: %.c $(DEPS)
      $(CC) -c -o $@ $< $(CFLAGS)

  lex: obj/main.o obj/reserved.o obj/linkedlist.o obj/token.o
      $(CC) $(CFLAGS) -o bin/lex $^

  .PHONY: clean
  clean:
      rm bin/*
      rm obj/*
```

# 10/15/2019

I have not worked any further on this as of today. I expect to have time in the
next couple of days to continue.

# 10/22/2019

I've begun project 2. All I have for now is the new productions resulting from
removing the 4 epsilon productions:

    1. <declarations> -> <declarations> var id: <type>; 
                         | ε
                         
    2. <subprogram_declarations> -> <subprogram_declarations> <subprogram_declaration>;
                                    | ε
                                    
    3. <arguments> -> (<parameter_list>)
                      | ε
                      
    4. <optional_statements> -> <statement_list>
                                | ε
# 11/4/2019
I've gotten project 1 further along. IDRES, WS, CATCHALL, INT, and RELOPS
machines are working. Still need to get REAL, LONGREAL written and I need
to change my Token struct to make use of the UNION construct for storing
ints OR pointers as attributes.

Also still have a lot of `#define`'s to clean up and organize into some
kind of system.


# 11/4/2019
Added INTTOOLONG and LEADINGZERO errors to Int machine. No for LONGREAL 
and REAL.

# 11/4/2019
Add LONGREAL and REAL machines without error checking.
