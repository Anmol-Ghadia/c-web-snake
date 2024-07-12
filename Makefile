# Compiler and flags
CC = gcc
EMCC = emcc

CFLAGS = -Wall -std=c99 -Iinclude
LDFLAGS = -LlibDesktop -lraylib -lopengl32 -lgdi32 -lwinmm

EMCCFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I.
EMLDFLAGS = -I D:/raylib/src -I D:/raylib/src/external -L. -L D:/raylib/src -s USE_GLFW=3 D:/raylib/src/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS='["_main","_give_touch_input","_give_key_input"]' -s EXPORTED_RUNTIME_METHODS=ccall,cwrap

# Source files
SRCS = $(wildcard src/*.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Output binaries
DESKTOP_TARGET = bin/desktop/main
WEB_TARGET = bin/web/main.html

# Default rule
all: desktop web

# Desktop build
desktop: $(DESKTOP_TARGET)

$(DESKTOP_TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Web build
web: $(WEB_TARGET)

$(WEB_TARGET): $(SRCS)
	$(EMCC) -o $@ $^ $(EMCCFLAGS) $(EMLDFLAGS)

# Compilation
src/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean
clean:
	rm -vf $(OBJS) bin/desktop/* bin/web/*

# Phony targets
.PHONY: all desktop web clean
