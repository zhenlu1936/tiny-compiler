SRCDIR := source
INCDIR := include
LEX_SRC := e.l
YACC_SRC := e.y
TARGET := e

# Source files
SRCS := $(SRCDIR)/e_helper.c \
        $(SRCDIR)/e_func.c   \
        $(SRCDIR)/e_stat.c   \
        $(SRCDIR)/e_exp.c    \
		e_main.c

# Generated files
LEX_C := e.l.c
YACC_C := e.y.c
YACC_H := e.y.h
YACC_OUT := e.y.output

CFLAGS := -I$(INCDIR) -g
LEX    := flex
YACC   := bison
CC     := gcc

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS) $(LEX_SRC) $(YACC_SRC)
	$(LEX) -o $(LEX_C) $(LEX_SRC)
	$(YACC) -d -v -o $(YACC_C) $(YACC_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_C) $(YACC_C) $(SRCS)

clean:
	rm -f $(TARGET) $(LEX_C) $(YACC_C) $(YACC_H) $(YACC_OUT)

test: all
	./$(TARGET) test.c
	make clean