CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lcrypto
SOURCES= main.c aes256.c io.c utils.c 
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE=ED-tool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
		$(CC) $(OBJECTS) $(LDFLAGS) -o $@


.c.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm *.o