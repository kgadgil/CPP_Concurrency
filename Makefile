CC = g++
CPPFLAGS = -std=c++11 -pthread
LDFLAGS =
SOURCES = tut1.cpp

tut1: tut1.cpp
	$(CC) $(CPPFLAGS) $? $(LDFLAGS) -o $@

clean:
	rm -f *.o tut1