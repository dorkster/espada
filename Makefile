PROJNAME=espada
SOURCES=src/main.c src/clips.c
DESTDIR?=/usr/local/games/$(PROJNAME)

CC?=gcc
CFLAGS?=-O2
CFLAGS+=`sdl-config --cflags` -Wall -mms-bitfields -std=c99
LDFLAGS+=`sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer -liniparser
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(PROJNAME)
RESOURCES=res/music.ogg res/background.png res/enemy_ship.png res/laser.png res/player_ship.png res/VeraBd.ttf res/explosion.wav res/player_fire.wav
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $< $ $(CFLAGS) -c -o $@

install:
	mkdir -p $(DESTDIR)/res
	cp -t $(DESTDIR)/res $(RESOURCES)
	cp $(EXECUTABLE) $(DESTDIR)
	chmod 755 $(DESTDIR)/$(PROJNAME)
	ln -s $(DESTDIR)/$(PROJNAME) /usr/bin/$(PROJNAME)

uninstall:
	rm -vfr $(DESTDIR)
	rm -vf /usr/bin/$(PROJNAME)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
