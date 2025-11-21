cc = gcc
cflag = -Wall -g

target = myprogram

obj = main.o mysum.o

all: $(target)

$(target): $(obj)
	$(cc) $(cflag) -o $(target) $(obj)

main.o: main.c mysum.h
	$(cc) $(cflag) -c main.c

mysum.o: mysum.c mysum.h
	$(cc) $(cflag) -c mysum.c

clean:
	rm -f $(target) $(obj)
