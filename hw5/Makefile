cc = gcc
cflag = -Wall

all: backend frontend

backend: backend.c common.h
	$(cc) $(cflag) -o backend backend.c

frontend: frontend.c common.h
	$(cc) $(cflag) -o frontend frontend.c

clean:
	rm -f backend frontend data.txt
