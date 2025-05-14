# Directories
SRCDIR := src
FRTDIR := $(SRCDIR)/frontend
BCKDIR := $(SRCDIR)/backend
ASMDIR := $(SRCDIR)/assemble
INCDIR := $(SRCDIR)/include
BUILDDIR := build
TESTDIR := test
TESTSRC := test

# Sources and generated files
MAIN_SRC := $(SRCDIR)/e_main.c

FRONT_SRCS := $(FRTDIR)/e_proc.c   \
		$(FRTDIR)/e_tac.c    
FRONT_INCS := $(INCDIR)/e_proc.h   \
		$(INCDIR)/e_tac.h    
LEX_FRONT_SRC := $(FRTDIR)/e.l
YACC_FRONT_SRC := $(FRTDIR)/e.y

BACK_SRCS := $(BCKDIR)/o_asm.c \
		$(BCKDIR)/o_reg.c \
		$(BCKDIR)/o_wrap.c 
BACK_INCS := $(INCDIR)/o_asm.h \
		$(INCDIR)/o_reg.h \
		$(INCDIR)/o_wrap.h     

MACHINE_SRCS := $(ASMDIR)/machine.c
BACK_INCS := $(INCDIR)/opcode.h
LEX_ASM_SRC := $(ASMDIR)/asm.l
YACC_ASM_SRC := $(ASMDIR)/asm.y

# Build outputs
LEX_FRONT_C := $(BUILDDIR)/e.l.c
YACC_FRONT_C := $(BUILDDIR)/e.y.c
YACC_FRONT_H := $(BUILDDIR)/e.y.h
YACC_FRONT_OUT := $(BUILDDIR)/e.y.output
LEX_ASM_C := $(BUILDDIR)/asm.l.c
YACC_ASM_C := $(BUILDDIR)/asm.y.c
YACC_ASM_H := $(BUILDDIR)/asm.y.h
YACC_ASM_OUT := $(BUILDDIR)/asm.y.output
ASM_OUT := $(BUILDDIR)/asm 
MACHINE_OUT := $(BUILDDIR)/machine
COMPILER_OUT := $(BUILDDIR)/e

# Tools and flags
LEX    := flex
YACC   := bison
CC     := gcc
CFLAGS := -I$(INCDIR) -g

.PHONY: all clean test

all: $(COMPILER_OUT) $(ASM_OUT) $(MACHINE_OUT)

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(COMPILER_OUT): $(FRONT_SRCS) $(BACK_SRCS) $(LEX_FRONT_SRC) $(YACC_FRONT_SRC) $(FRONT_INCS) $(BACK_INCS) | $(BUILDDIR)
	$(LEX) -o $(LEX_FRONT_C) $(LEX_FRONT_SRC)
	$(YACC) -d -v -o $(YACC_FRONT_C) $(YACC_FRONT_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_FRONT_C) $(YACC_FRONT_C) $(FRONT_SRCS) $(BACK_SRCS) $(MAIN_SRC)

$(ASM_OUT): $(LEX_ASM_SRC) $(YACC_ASM_SRC) | $(BUILDDIR)
	$(LEX) -o $(LEX_ASM_C) $(LEX_ASM_SRC)
	$(YACC) -d -v -o $(YACC_ASM_C) $(YACC_ASM_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_ASM_C) $(YACC_ASM_C)

$(MACHINE_OUT): $(MACHINE_SRCS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(MACHINE_SRCS)

clean:
	rm -rf $(BUILDDIR) $(TESTDIR)/*.s $(TESTDIR)/*.x $(TESTDIR)/*.o

test: all
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH; \
	./$(COMPILER_OUT) ./$(TESTDIR)/$(TESTSRC).c
	./$(ASM_OUT) ./$(TESTDIR)/$(TESTSRC).s
	./$(MACHINE_OUT) ./$(TESTDIR)/$(TESTSRC).o