all: swifi.out setcheck.out
swifi.out: swifi.c
	cc -Weverything -Wno-poison-system-directories -Werror swifi.c -o swifi.out
setcheck.out: setcheck.c
	cc -Weverything -Wno-poison-system-directories -Werror setcheck.c -o setcheck.out
