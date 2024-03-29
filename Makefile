CC = clang
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic #-fsanitize=address -g
DEPS = reserved.h linkedlist.h output.h parse.h colornode.h stack.h

VPATH = src:src/lex/:src/parse/:src/util/:obj
vpath %.h src/headers/
vpath %.o obj

obj/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pasc: obj/main.o obj/reserved.o obj/linkedlist.o obj/token.o obj/output.o obj/colornode.o obj/stack.o obj/parse.o
	$(CC) $(CFLAGS) -o bin/pasc $^

v:
	make clean && make && \
	./bin/lex > output.txt && \
	bat output.txt

test:
	make clean && make && \
	./bin/lex > output.txt && \
	cat output.txt|grep -i error || echo -e '\n0 syntax errors'

.PHONY: clean
clean:
	$(RM) bin/*
	$(RM) obj/*
	$(RM) listingfile
	$(RM) tokenfile
	$(RM) symboltable
	$(RM) output.txt

