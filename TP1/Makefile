# Makefile for building the OpenGL project

CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lGL -lGLU -lglut

# Source files
SRCS = main.c global.c feature1.c
OBJS = $(SRCS:.c=.o)
TARGET = my_opengl_app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

