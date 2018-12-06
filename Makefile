## BASIC MAKEFILE ##
# General syntax of a Makefile
# target [target ...] : [dependent ...]
#<tab>[command...]


CC = g++
CPPFLAGS = -std=c++11 -pthread -ggdb
#LDFLAGS
SOURCES = gr_circular_file.cpp
#SOURCES = bounded_q.cpp
#SOURCES = lockfree_spsc.cpp

all: tut run

tut: $(SOURCES)
	$(CC) $(CPPFLAGS) $(SOURCES) $(LDFLAGS) -o $@

run: tut
	./tut 1024 1024 1024

clean:
	rm -f *.o tut