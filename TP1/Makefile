CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
TARGET  = main
OBJS    = main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c GUI.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) $(TARGET)
