CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=receiver.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=receiver

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
