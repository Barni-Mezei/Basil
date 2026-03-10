all:
	gcc src/c/main.c -Wall -o bin/main -I/usr/local/lua/include -L/usr/local/lua -llua /usr/local/lib/liblua.a

clear:
	rm -f main.o
	rm -f main
