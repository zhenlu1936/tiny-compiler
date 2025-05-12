# Directories
SRCDIR := src
FRTDIR := $(SRCDIR)/frontend
INCDIR := $(SRCDIR)/include
BUILDDIR := build
TESTDIR := test

# Sources and headers
SRCS := $(FRTDIR)/e_proc.c   \
        $(FRTDIR)/e_tac.c    \
        $(FRTDIR)/e_main.c
INCS := $(INCDIR)/e_proc.h   \
        $(INCDIR)/e_tac.h

# Shared library sources
LIB_SRCS := $(FRTDIR)/reg_manager.c \
            $(FRTDIR)/asm_generator.c \
            $(FRTDIR)/code_generator.c

# Shared library outputs
LIBS := $(BUILDDIR)/libreg_manager.so \
        $(BUILDDIR)/libasm_generator.so \
        $(BUILDDIR)/libcode_generator.so

# Build outputs
LEX_SRC := $(FRTDIR)/e.l
YACC_SRC := $(FRTDIR)/e.y
LEX_C := $(BUILDDIR)/e.l.c
YACC_C := $(BUILDDIR)/e.y.c
YACC_H := $(BUILDDIR)/e.y.h
YACC_OUT := $(BUILDDIR)/e.y.output
TARGET := $(BUILDDIR)/e

# Tools and flags
LEX    := flex
YACC   := bison
CC     := gcc
CFLAGS := -I$(INCDIR) -g -fPIC
LFLAGS := -undefined dynamic_lookup
LOAD_SO := -L$(BUILDDIR) -lreg_manager -lasm_generator -lcode_generator

.PHONY: all clean test

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Build target
all: $(BUILDDIR) $(LIBS) $(TARGET)

# Build shared libraries
$(BUILDDIR)/libreg_manager.so: $(FRTDIR)/reg_manager.c $(INCDIR)/reg_manager.h
	$(CC) $(CFLAGS) -shared -o $@ $< $(LFLAGS)

$(BUILDDIR)/libasm_generator.so: $(FRTDIR)/asm_generator.c $(INCDIR)/asm_generator.h
	$(CC) $(CFLAGS) -shared -o $@ $< -L$(BUILDDIR) -lreg_manager $(LFLAGS)

$(BUILDDIR)/libcode_generator.so: $(FRTDIR)/code_generator.c $(INCDIR)/code_generator.h
	$(CC) $(CFLAGS) -shared -o $@ $< -L$(BUILDDIR) -lreg_manager -lasm_generator $(LFLAGS)

# Build main target
$(TARGET): $(SRCS) $(LEX_SRC) $(YACC_SRC) $(INCS) | $(BUILDDIR)
	$(LEX) -o $(LEX_C) $(LEX_SRC)
	$(YACC) -d -v -o $(YACC_C) $(YACC_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEX_C) $(YACC_C) $(SRCS) $(LOAD_SO)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(TESTDIR)/*.s $(TESTDIR)/*.x

# Run tests
test: all
	./$(TARGET) ./$(TESTDIR)/test.c