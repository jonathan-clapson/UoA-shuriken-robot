SOURCES=main.c
HEADERS=

CFLAGS=-Wall -std=gnu99
LIBS=-lpthread -lbcm2835 -lm

all: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -o main $(SOURCES) $(LIBS)

debug: $(SOURCES) $(HEADERS)
	gcc -g $(CFLAGS) -o main $(SOURCES) $(LIBS)


