CC = clang
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
DEPS = reserved.h linkedlist.h

VPATH = src:src/lex/:obj
vpath %.h src/headers/
vpath %.o obj

obj/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

readline: obj/main.o obj/reserved.o obj/linkedlist.o obj/token.o
	$(CC) $(CFLAGS) -o bin/readline $^

.PHONY: clean
clean:
	rm bin/*
	rm obj/*
