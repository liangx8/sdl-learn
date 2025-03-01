.PHONY: clean
CC=gcc
RM=rm -rf
#CFLAGS+=$(shell pkgconf sdl2 --cflags)
CFLAGS=-Wall
TARGET=sdl
ifeq (${OPTS}x,x)
	CFLAGS+=-O2
else
	CFLAGS+=${OPTS}
endif

SRCS=$(wildcard src/*.c)
OBJS=${addprefix obj/,${notdir ${SRCS:.c=.o}}}
LDFLAGS=-Wall
LDFLAGS+=-lSDL2
LDFLAGS+=-lSDL2_ttf
LDFLAGS+=# -static

all:mkobj ${TARGET}
	@echo "*** Done ***"
release:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${LDFLAGS}
	strip	${TARGET}
	@echo "*** Done ***"
${TARGET}:${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}
clean:
	${RM} ${TARGET} *~
	rm -rf obj
#%.o:%.cpp
obj/%.o:src/%.c
	${CC} -c $< -o $@ ${CFLAGS}

mkobj:
	mkdir -p obj
