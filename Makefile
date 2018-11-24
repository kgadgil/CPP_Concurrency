## BASIC MAKEFILE ##
# General syntax of a Makefile
# target [target ...] : [dependent ...]
#<tab>[command...]


CC = g++
CPPFLAGS = -std=c++11 -pthread
LDFLAGS =
SOURCES = tut1.cpp


tut1: $(SOURCES)
	$(CC) $(CPPFLAGS) $(SOURCES) $(LDFLAGS) -o $@

run: tut1
	./tut1

clean:
	rm -f *.o tut1