IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm


_OBJ = 1b.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

1b.out: 1b.c
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: 1b.c
		gcc -o $@ $^ $(CFLAGS) $(LIBS)
.PHONY: clean

clean:
		rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

