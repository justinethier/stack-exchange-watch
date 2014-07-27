CC = gcc
CSTD = -std=c99
CFLAGS = $(CSTD) -g

COBJ = stack-exchange-poll http se-api util
CFILES = $(addsuffix .c, $(COBJ))
#COBJS = $(addsuffix .o, $(COBJ))

# Requires json-c library
#  on Ubuntu: sudo apt-get install libjson0 libjson0-dev
# And libcurl
#  install on Ubuntu: sudo apt-get install libcurl4-openssl-dev
stack-exchange-poll: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -lcurl -ljson -o stack-exchange-poll

.PHONY: clean
clean:
	rm -rf a.out http stack-exchange-poll *.o
