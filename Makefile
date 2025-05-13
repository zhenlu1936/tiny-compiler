# Directories
SRCDIR := src
FRTDIR := $(SRCDIR)/frontend
BCKDIR := $(SRCDIR)/backend
INCDIR := $(SRCDIR)/include
BUILDDIR := build
TESTDIR := test
TESTSRC := test

# Sources and generated files
FRONT_SRCS := $(FRTDIR)/e_proc.c   \
		$(FRTDIR)/e_tac.c    \
		$(FRTDIR)/e_obj.c	 \
		$(FRTDIR)/e_main.c	 
FRONT_INCS := $(INCDIR)/e_proc.h   \
		$(INCDIR)/e_tac.h    
LEX_FRONT_SRC := $(FRTDIR)/e.l
YACC_FRONT_SRC := $(FRTDIR)/e.y

MACHINE_SRCS := $(BCKDIR)/machine.c
BACK_INCS := $(INCDIR)/opcode.h
LEX_ASM_SRC := $(BCKDIR)/asm.l
YACC_ASM_SRC := $(BCKDIR)/asm.y

# Build outputs
LEX_FRONT_C := $(BUILDDIR)/e.l.c
YACC_FRONT_C := $(BUILDDIR)/e.y.c
YACC_FRONT_H := $(BUILDDIR)/e.y.h
YACC_FRONT_OUT := $(BUILDDIR)/e.y.output
FRONT_OUT := $(BUILDDIR)/e
LEX_ASM_C := $(BUILDDIR)/asm.l.c
YACC_ASM_C := $(BUILDDIR)/asm.y.c
YACC_ASM_H := $(BUILDDIR)/asm.y.h
YACC_ASM_OUT := $(BUILDDIR)/asm.y.output
ASM_OUT := $(BUILDDIR)/asm 
MACHINE_OUT := $(BUILDDIR)/machine
BACK_OUT := $(ASM_OUT) $(MACHINE_OUT)

# Tools and flags
LEX    := flex
YACC   := bison
CC     := gcc
CFLAGS := -I$(INCDIR) -g

.PHONY: all clean test

all: $(FRONT_OUT) $(BACK_OUT)

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(FRONT_OUT): $(FRONT_SRCS) $(LEX_FRONT_SRC) $(YACC_FRONT_SRC) $(FRONT_INCS) | $(BUILDDIR)
	$(LEX) -o $(LEX_FRONT_C) $(LEX_FRONT_SRC)
	$(YACC) -d -v -o $(YACC_FRONT_C) $(YACC_FRONT_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_FRONT_C) $(YACC_FRONT_C) $(FRONT_SRCS)

$(ASM_OUT): $(LEX_ASM_SRC) $(YACC_ASM_SRC) $(BACK_INCS) | $(BUILDDIR)
	$(LEX) -o $(LEX_ASM_C) $(LEX_ASM_SRC)
	$(YACC) -d -v -o $(YACC_ASM_C) $(YACC_ASM_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_ASM_C) $(YACC_ASM_C)

$(MACHINE_OUT): $(MACHINE_SRCS) $(BACK_INCS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(MACHINE_SRCS)

clean:
	rm -rf $(BUILDDIR) $(TESTDIR)/*.s $(TESTDIR)/*.x $(TESTDIR)/*.o

test: all
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH; \
	./$(FRONT_OUT) ./$(TESTDIR)/$(TESTSRC).c
	./$(ASM_OUT) ./$(TESTDIR)/$(TESTSRC).s
	./$(MACHINE_OUT) ./$(TESTDIR)/$(TESTSRC).o