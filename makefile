.POSIX:

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	CC = cc
	# Apple default compiler is clang; removed -Wno-poison... flag
	# because of bug after updating Xcode (along with clang 12.0.0)
	CFLAGS = -Weverything -Wno-poison-system-directories -Werror
endif
ifeq ($(UNAME),Linux)
	CC = cc
	# Lubuntu default compile is gcc
	CFLAGS = -Wall -Wextra -pedantic -Werror
endif

all: swifi.out setcheck.out
swifi.out: swifi.c
	$(CC) $(CFLAGS) swifi.c -o swifi.out
setcheck.out: setcheck.c
	$(CC) $(CFLAGS) setcheck.c -o setcheck.out
clean: 
	rm swifi.out setcheck.out
