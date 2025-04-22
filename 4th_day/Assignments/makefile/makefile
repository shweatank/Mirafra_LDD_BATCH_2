CC = gcc
CFLAGS = -Wall -g

main.out: main.o add.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c add.h
	$(CC) $(CFLAGS) -c $< -o $@

add.o: add.c add.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o main.out

