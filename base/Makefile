CC      = gcc -Wall -O2
O_FILES = block.o dc.o error.o ident.o keywords.o lowlevel.o object.o parser.o \
	  relation.o scanner.o symbol.o

dc: $(O_FILES)
	$(CC) -o $@ $(O_FILES)

.c.o:
	$(CC) -c $<

clean:
	rm -f *~ *.o dc
