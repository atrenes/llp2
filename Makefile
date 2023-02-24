.PHONY: all parser clean
all: parser.y tokenizer.l
	make parser
	make run

light:
	gcc -g -o main parser.tab.c lex.yy.c tree.c printer.c

parser:
	bison -d parser.y
	flex tokenizer.l
	make light

run:
	./main

clean:
	rm -f program lex.yy.c parser.tab.c parser.tab.h