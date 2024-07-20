.PHONY: clean
CC=gcc
RM=rm -rf
#CFLAGS+=$(shell pkgconf sdl2 --cflags)
CFLAGS=-Wall
CFLAGS+=-D_REENTRANT
TARGET=sdl
ifeq (${OPTS}x,x)
	CFLAGS+=-O2
else
	CFLAGS+=${OPTS}
endif

SRCS=$(wildcard *.c)
SRCS+=$(wildcard cengin/*.c)
OBJS=${SRCS:.c=.o}
LDFLAGS=-Wall
LDFLAGS+=-lSDL2
LDFLAGS+=-lSDL2_image
LDFLAGS+=-lSDL2_ttf
LDFLAGS+=# -static

all:${TARGET}
	@echo "*** Done ***"
release:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${LDFLAGS}
	strip	${TARGET}
	@echo "*** Done ***"
${TARGET}:${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}
clean:
	${RM} ${TARGET} *~
	find . -name \*.o -exec ${RM} \{} \;

#%.o:%.cpp
.c.o:
	${CC} -c $< -o $@ ${CFLAGS}

