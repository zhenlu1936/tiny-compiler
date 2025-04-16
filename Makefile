e: e_helper.c e_helper.h e_generator.h e_generator.c e.l e.y
	flex -o e.l.c e.l
	bison -d -v -o e.y.c e.y
	gcc -o e e.l.c e.y.c e_helper.c e_generator.c -g

clean:
	rm -fr e e.l.c e.y.c e.y.h e.y.output

test: e
	./e test.c
	make clean

.PHONY: clean test