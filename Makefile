all: lex.lex parse.c
	flex lex.lex
	gcc -c lex.yy.c
	gcc -c parse.c
	gcc -o parser lex.yy.o parse.o 


clean:
	rm -rf *.o simp.c lex.yy.c parser
