CC = clang
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
DEPS = relops.h

VPATH = src:src/lex/:obj
vpath %.h src/headers/
vpath %.o obj

obj/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


readline: obj/readline.o obj/relops.o
	$(CC) $(CFLAGS) -o bin/readline $^

.PHONY: clean
clean:
	rm bin/*
	rm obj/*
