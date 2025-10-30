CC = gcc

TARGET = main

all: $(TARGET)

$(TARGET): main.o foo1.o foo2.o
	$(CC) -g -o $(TARGET) main.o foo1.o foo2.o

main.o: main.c
	$(CC) -g -c main.c

foo1.o: foo1.c
	$(CC) -g -c foo1.c

foo2.o: foo2.c
	$(CC) -g -c foo2.c

%.s: %.c
	$(CC) -g -S $<

clean:
	rm -f *.o*.s $(TARGET)
