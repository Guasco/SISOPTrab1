make: testes_thread.c ../lib/libuthread.a
	gcc -g -Wall -c testes_thread.c -o ../bin/testes_thread.o
	gcc -o ../bin/testes_thread ../bin/testes_thread.o -L../lib -luthread

clean:
	rm ../bin/testes_thread ../bin/testes_thread.o
