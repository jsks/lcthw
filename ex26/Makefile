PREFIX?=usr/local
CFLAGS=-g -v -Wall -I${PREFIX}/include/apr-1
LDFLAGS=-L${PREFIX}/lib -lapr-1 -pthread -laprutil-1

all: devpkg

devpkg: bstrlib.o db.o shell.o commands.o

install: all
	install -d $(PREFIX)/bin/
	install devpkg $(PREFIX)/bin/

clean:
	rm -f *.o
	rm -f devpkg
	rm -rf *.dSYM
