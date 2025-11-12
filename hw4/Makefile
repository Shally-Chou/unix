target = user_list
cc = gcc
cflag = -Wall -g

all: $(target)

$(target): user_list.c
	$(cc) $(cflag) -o $@ $^

clean:
	rm -f $(target)
