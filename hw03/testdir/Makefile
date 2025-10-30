cc = gcc

target = hw_03

cflag = -Wall -g

src = hw_03.c

all: $(target)

$(target): $(src:.c=.o)
	$(cc) $(cflag) -o $(target) $(src:.c=.o)

%.o: %.c
	$(cc) $(cflag) -c $< -o $@

clean:
	rm -f $(target)*.o

.PHONY: all clean
