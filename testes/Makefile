make: testes_thread.c filatestes.c ../lib/libuthread.a
	gcc -g -Wall -c testes_thread.c -o ../bin/testes_thread.o
	gcc -g -Wall -c filatestes.c -o ../bin/filatestes.o
	gcc -o ../bin/testes_thread ../bin/testes_thread.o -L../lib -luthread
	gcc -o ../bin/filatestes ../bin/filatestes.o -L../lib -luthread

clean:
	rm ../bin/testes_thread ../bin/testes_thread.o ../bin/filatestes ../bin/filatestes.o
