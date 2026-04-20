CFLAGS=-std=c11 -g -static -DDEBUG_PARSE
SRCS=main.c 
OBJS=$(SRCS:.c=.o)

mini_cc: $(OBJS)
	$(CC) -o mini_cc $(OBJS)

ext.o: ext.c
	$(CC) -c ext.c -o ext.o

run: mini_cc
	./mini_cc

run_args: mini_cc ext.o
	bash test.sh

clean:
	rm -f mini_cc $(OBJS) temps ext.o

.PHONY: run ext.o run_args clean 
