CC=gcc
CFLAGS=-c -Wall -g -Iinclude
SRCDIR=src
BUILDDIR=build
EXECDIR=bin
LDFLAGS=-lcrypto
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
EXECUTABLE=ED-tool

all: clean dir $(SOURCES) $(EXECDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(EXECDIR) $(BUILDDIR)

$(EXECDIR)/$(EXECUTABLE):$(OBJECTS)
		$(CC) $^ $(LDFLAGS) -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SRCDIR)/%.c
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf $(BUILDDIR) $(EXECDIR)