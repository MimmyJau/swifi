.POSIX:

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	CC = cc
	CFLAGS = -Weverything -Wno-poison-system-directories -Werror
endif
ifeq ($(UNAME),Linux)
	CC = cc
	CFLAGS = -Wall -Wextra -pedantic -Werror
endif

all: swifi.out setcheck.out
swifi.out: swifi.c
	$(CC) $(CFLAGS) swifi.c -o swifi.out
setcheck.out: setcheck.c
	$(CC) $(CFLAGS) setcheck.c -o setcheck.out
clean: 
	rm swifi.out setcheck.out
