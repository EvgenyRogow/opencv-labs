CC = g++
CFLAGS = -g
SOURCES = main.cpp
PROG = main 

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SOURCES) $(LIBS)
