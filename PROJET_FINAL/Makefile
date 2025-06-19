# Compiler and flags
CC       := gcc
CFLAGS   := -Wall -Wextra -Iinclude -std=c99


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -framework OpenGL -framework GLUT
else
    LDFLAGS = -lGL -lGLU -lglut -lm
endif



# Directories
SRCDIR   := src
OBJDIR   := obj

# Source and object files
SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Final executable
TARGET   := renderer

.PHONY: all clean

all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c into obj/.o
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure object directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	rm -rf $(OBJDIR) $(TARGET)

