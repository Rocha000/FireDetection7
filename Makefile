CC = gcc
CFLAGS = -Wall -pthread

all: fire_detection

fire_detection: main.o functions.o
	$(CC) $(CFLAGS) -o fire_detection main.o functions.o

main.o: main.c functions.h
	$(CC) $(CFLAGS) -c main.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

clean:
	rm -f *.o fire_detection
