.PHONY: all clean

CC := gcc
RM := rm -rf
TARGET := sdl

# Optional optimization/flags from command line: make OPTC=-g
CFLAGS := -Wall -Wextra -std=c11
ifeq ($(OPTC),)
CFLAGS += -O2
else
CFLAGS += $(OPTC)
endif

# Hardcoded SDL2 include and link flags.
# Adjust these paths if SDL2 is installed in a custom location.
SDL_CFLAGS := -I/usr/include/SDL2
SDL_LDFLAGS := -L/usr/lib -lSDL2 -lSDL2_ttf

CFLAGS += $(SDL_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: $(TARGET)
	@echo "Build complete: $(TARGET)"

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)
