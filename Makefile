CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = hencode hdecode
OBJS = hencode.o hdecode.o 
objects = hencode hdecode
all : $(objects)

$(objects): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

hencode.o : hencode.c hencode.h htable.h
	$(CC) $(CFLAGS) -c hencode.c

hdecode.o : hdecode.c hdecode.h htable.h
	$(CC) $(CFLAGS) -c hdecode.c

htable.o : htable.c htable.h
	$(CC) $(CFLAGS) -c htable.c

clean :
	rm *.0 $(MAIN) core*
