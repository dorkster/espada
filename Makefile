PROJNAME=game
SOURCES=src/main.c
ifndef DESTDIR
DESTDIR=/usr/local/bin
endif

CC=gcc
CFLAGS+=
LDFLAGS+=-lSDL -lSDL_image -lSDL_ttf
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/$(PROJNAME)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p bin
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall -O2 -mms-bitfields -std=c99 -o $@

.c.o:
	$(CC) $< $ $(CFLAGS) -c -Wall -O2 -mms-bitfields -std=c99 -o $@

install:
	cp $(EXECUTABLE) $(DESTDIR)
	chmod 755 $(DESTDIR)/$(PROJNAME)

uninstall:
	rm -i $(DESTDIR)/$(PROJNAME)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
