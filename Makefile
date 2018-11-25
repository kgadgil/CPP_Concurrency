## BASIC MAKEFILE ##
# General syntax of a Makefile
# target [target ...] : [dependent ...]
#<tab>[command...]


CC = g++
CPPFLAGS = -std=c++11 -pthread
LDFLAGS =
SOURCES = tut4.cpp

all: tut run clean

tut: $(SOURCES)
	$(CC) $(CPPFLAGS) $(SOURCES) $(LDFLAGS) -o $@

run: tut
	./tut 1

clean:
	rm -f *.o tut