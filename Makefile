CC=gcc
CFLAGS=-c -wAll
LDFLAGS=
SOURCES= main.c aes256.c io.c utils.c 
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE=ED-tool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@


.c.o:
		$(CC) $(CFLAGS) $< -o $@