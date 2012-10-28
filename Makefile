make: ./src/uthread.c 
	gcc -g -Wall -c ./src/uthread.c -o ./bin/uthread.o
	ar crs ./lib/libuthread.a ./bin/uthread.o

clean:
	rm ./bin/uthread.o ./lib/libuthread.a
