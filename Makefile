CC = gcc
CFLAGS = -Wall
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

clean:
	rm -f $(TARGET)

.PHONY: all run build clean
