CFLAGS=-std=c11 -g -static

slcc: main.c
	cc $(CFLAGS) -o slcc $<

test: slcc
	./test.sh

clean:
	rm -f slcc *.o tmp*

.PHONY: test clean
