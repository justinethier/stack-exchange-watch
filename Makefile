CC = gcc
CSTD = -std=c99
CFLAGS = $(CSTD) -g

COBJ = se-api util main http 
CFILES = $(addsuffix .c, $(COBJ))
COBJECTS=$(CFILES:.c=.o)

TOBJ = se-api util test
TFILES = $(addsuffix .c, $(TOBJ))
TOBJECTS=$(TFILES:.c=.o)

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@

# Requires json-c library
#  on Ubuntu: sudo apt-get install libjson0 libjson0-dev
# And libcurl
#  install on Ubuntu: sudo apt-get install libcurl4-openssl-dev
stack-watch: $(COBJECTS)
	$(CC) $(CFLAGS) $(COBJECTS) -lcurl -ljson -o stack-watch

test: $(TOBJECTS)
	$(CC) $(CFLAGS) $(TOBJECTS) -ljson -o stack-tests
	./stack-tests

.PHONY: clean
clean:
	rm -rf a.out http stack-watch stack-tests *.o
