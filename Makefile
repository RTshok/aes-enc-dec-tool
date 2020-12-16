CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-lcrypto
SOURCES= main.c aes256.c io.c utils.c 
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE=ED-tool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
		$(CC) $(OBJECTS) $(LDFLAGS) -o $@
		mkdir ./build ./bin
		mv *.o ./build
		mv $@ ./bin


.c.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm *.o
		rm -r ./build
		rm -r ./bin