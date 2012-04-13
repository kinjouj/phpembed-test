PHP_DIR = $(shell php-config --prefix)

compile:
	gcc -g server.cc -o server -I. `php-config --libs` `php-config --includes` -L$(PHP_DIR)/lib -lphp5 -levent

run:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PHP_DIR)/lib ./server

gdb:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PHP_DIR)/lib gdb ./bin/client

valgrind:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PHP_DIR)/lib valgrind ./bin/client

clean:
	rm -fr bin/client
