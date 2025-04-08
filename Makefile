e: lex.yy.o e.tab.o e_helper.o
	flex e.l
	bison -d -v e.y
	gcc -o e lex.yy.c e.tab.c e_helper.c

lex.yy.o: e_helper.o e.tab.o e.l
	flex e.l
	gcc -c lex.yy.c -o lex.yy.o

e.tab.o: e_helper.o e.y
	bison -d e.y
	gcc -c e.tab.c -o e.tab.o

e_helper.o: e_helper.c
	gcc -c e_helper.c -o e_helper.o

clean:
	rm e
	rm e.tab.c
	rm e.tab.h
	rm lex.yy.c
	rm e.output
	rm *.o

test: e
	./e test.c

.PHONY: clean test