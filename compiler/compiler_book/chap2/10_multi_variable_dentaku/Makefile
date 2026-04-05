CFLAGS=-std=c11 -g -static
SRCS=main.c parse.c codegen.c
OBJS=$(SRCS:.c=.o)

dentaku: $(OBJS)
	$(CC) -o dentaku $(OBJS) $(LDFLAGS)

$(OBJS): dentaku.h

test: dentaku
	bash test.sh

clean:
	rm -f dentaku *.o *~ temp*

.PHONY: test clean 
