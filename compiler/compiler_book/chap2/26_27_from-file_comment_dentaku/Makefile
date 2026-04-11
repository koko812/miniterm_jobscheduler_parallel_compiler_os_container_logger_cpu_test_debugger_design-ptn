CFLAGS=-std=c11 -g -static
SRCS=main.c parse.c codegen.c
OBJS=$(SRCS:.c=.o)

dentaku: $(OBJS)
	$(CC) -o dentaku $(OBJS) $(LDFLAGS)

$(OBJS): dentaku.h

ext.o: ext.c
	$(CC) -c ext.c -o ext.o

test: dentaku ext.o
	bash test_from_file.sh

clean:
	rm -f dentaku *.o *~ temp*

.PHONY: test clean 
