# Directories
SRCDIR := source
INCDIR := include
BUILDDIR := build

# Sources and generated files
SRCS := $(SRCDIR)/e_helper.c \
        $(SRCDIR)/e_func.c   \
        $(SRCDIR)/e_stat.c   \
        $(SRCDIR)/e_exp.c    \
		e_main.c
LEX_SRC := e.l
YACC_SRC := e.y

# Build outputs
LEX_C := $(BUILDDIR)/e.l.c
YACC_C := $(BUILDDIR)/e.y.c
YACC_H := $(BUILDDIR)/e.y.h
YACC_OUT := $(BUILDDIR)/e.y.output
TARGET := $(BUILDDIR)/e

# Tools and flags
LEX    := flex
YACC   := bison
CC     := gcc
CFLAGS := -I$(INCDIR) -g

.PHONY: all clean test

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

all: $(TARGET)

$(TARGET): $(SRCS) $(LEX_SRC) $(YACC_SRC) | $(BUILDDIR)
	$(LEX) -o $(LEX_C) $(LEX_SRC)
	$(YACC) -d -v -o $(YACC_C) $(YACC_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_C) $(YACC_C) $(SRCS)

clean:
	rm -rf $(BUILDDIR)

test: all
	./$(TARGET) test.c
	$(MAKE) clean
