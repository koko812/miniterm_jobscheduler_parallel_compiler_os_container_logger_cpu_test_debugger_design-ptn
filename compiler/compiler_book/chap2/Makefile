CFLAGS=-std=c11 -g -static

dentaku: dentaku.c

test: dentaku
	bash test.sh

clean:
	rm -f dentaku *.o *~ temp*

.PHONY: test clean 
