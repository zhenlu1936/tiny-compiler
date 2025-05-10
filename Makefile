# Directories
SRCDIR := src
FRTDIR := $(SRCDIR)/frontend
BCKDIR := $(SRCDIR)/backend
INCDIR := $(SRCDIR)/include
BUILDDIR := build
TESTDIR := test

# Sources and generated files
SRCS := $(FRTDIR)/e_proc.c   \
		$(FRTDIR)/e_tac.c    \
		$(FRTDIR)/e_obj.c	 \
		$(FRTDIR)/e_main.c	 
INCS := $(INCDIR)/e_proc.h   \
		$(INCDIR)/e_tac.h    
LEX_SRC := $(FRTDIR)/e.l
YACC_SRC := $(FRTDIR)/e.y

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

$(TARGET): $(SRCS) $(LEX_SRC) $(YACC_SRC) $(INCS) | $(BUILDDIR)
	$(LEX) -o $(LEX_C) $(LEX_SRC)
	$(YACC) -d -v -o $(YACC_C) $(YACC_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_C) $(YACC_C) $(SRCS)

clean:
	rm -rf $(BUILDDIR) $(TESTDIR)/*.s $(TESTDIR)/*.x

test: all
	./$(TARGET) ./$(TESTDIR)/test.c
# $(MAKE) clean
