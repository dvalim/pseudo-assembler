IDIR =./include
SRCDIR =./src
CC=gcc
CFLAGS=-I$(IDIR) -O2

ODIR=obj

LIBS=-lm

_DEPS = data_structures.h interpreter.h console.h error_handling.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = data_structures.o interpreter.o console.o error_handling.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

interpreter: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 