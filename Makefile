all: server.o /usr/local/lib/libuv.a
	gcc -o main server.c /usr/local/lib/libuv.a

server: server.c
	gcc -c server.c

libuv:
	cd libuv && $(MAKE) install
