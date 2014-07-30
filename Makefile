CC = gcc
CSTD = -std=c99
CFLAGS = $(CSTD) -g

COBJ = se-api util
CFILES = $(addsuffix .c, $(COBJ))
#COBJS = $(addsuffix .o, $(COBJ))

# Requires json-c library
#  on Ubuntu: sudo apt-get install libjson0 libjson0-dev
# And libcurl
#  install on Ubuntu: sudo apt-get install libcurl4-openssl-dev
stack-watch: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) main.c http.c -lcurl -ljson -o stack-watch

.PHONY: test
test:
	$(CC) $(CFLAGS) $(CFILES) test.c -ljson -o stack-tests
	./stack-tests

.PHONY: clean
clean:
	rm -rf a.out http stack-watch *.o
