CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=sender.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sender

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
