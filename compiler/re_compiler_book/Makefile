CFLAGS=-std=c11 -g -static -DDEBUG_PARSE
SRCS=main.c 
OBJS=$(SRCS:.c=.o)

mini_cc: $(OBJS)
	$(CC) -o mini_cc $(OBJS)

run: mini_cc
	./mini_cc

run_args: mini_cc
	bash test.sh

clean:
	rm -f mini_cc $(OBJS) temps

.PHONY: run run_args clean 
