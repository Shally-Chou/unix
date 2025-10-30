hw2: main.o libmyfstream.a
	g++ -g -o hw2 main.o -L. -lmyfstream

%.o: %.cpp
	g++ -g -c $<

libmyfstream.a: myfstream.o
	ar rcs $@ myfstream.o

clean:
	rm -f *.o libmyfstream.a hw2
