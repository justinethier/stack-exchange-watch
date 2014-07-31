# 
# This file is part of stack-watch, a utility to monitor Stack Exchange.
# Copyright (C) 2014 Justin Ethier <justinethier@github>
# 
# Please use github to submit patches and bug reports:
# https://github.com/justinethier/stack-exchange-watch
# 
CC = gcc
CSTD = -std=c99
CFLAGS = $(CSTD) -g

COBJ = se-api util main http 
CFILES = $(addsuffix .c, $(COBJ))
COBJECTS=$(CFILES:.c=.o)

TOBJ = se-api util test
TFILES = $(addsuffix .c, $(TOBJ))
TOBJECTS=$(TFILES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

stack-watch: $(COBJECTS)
	$(CC) $(CFLAGS) $(COBJECTS) -lcurl -ljson -o stack-watch

test: $(TOBJECTS)
	$(CC) $(CFLAGS) $(TOBJECTS) -ljson -o stack-tests
	./stack-tests

.PHONY: clean
clean:
	rm -rf a.out http stack-watch stack-tests *.o
