CC=clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LIBFLAGS = -lncurses

all: life 

life: life.o universe.o
	$(CC) $(CFLAGS) $(LIBFLAGS) -o life life.o universe.o
	
universe.o: universe.c
	$(CC) $(CFLAGS) -c universe.c
	
clean:
	rm -f life *.o

format:
	clang-format -i -style=file *.[c,h]
