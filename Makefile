PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

CC = gcc
CFLAGS = -O2 -Wall
LDLIBS = -lasound -lm
TARGET = abeep
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: run

run: build
	./$(TARGET)

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDLIBS)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run build clean
