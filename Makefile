# Directories
SRCDIR := src
INCDIR := $(SRCDIR)/include
BUILDDIR := build
TESTDIR := test

# Sources and generated files
SRCS := $(SRCDIR)/e_proc.c   \
		$(SRCDIR)/e_tac.c    \
		$(SRCDIR)/e_main.c	 
LEX_SRC := $(SRCDIR)/e.l
YACC_SRC := $(SRCDIR)/e.y

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
	rm -rf $(BUILDDIR) tac.txt

test: all
	./$(TARGET) ./$(TESTDIR)/test.c
# $(MAKE) clean
