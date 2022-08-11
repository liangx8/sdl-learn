.PHONY: clean
CC=gcc
RM=rm -rf
CFLAGS=-Wall -g
CFLAGS+=$(shell sdl2-config --cflags)
TARGET=sdl


SRCS=$(shell ls *.c)
OBJS=${SRCS:.c=.o}
LDFLAGS=-Wall -g
LDFLAGS+=$(shell sdl2-config --libs)
LDFLAGS+=# -static

all:${TARGET}
	@echo "*** Done ***"
release:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${LDFLAGS} -static
	strip	${TARGET}
	@echo "*** Done ***"
${TARGET}:${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}
clean:
	${RM} *.o ${TARGET} *~

#%.o:%.cpp
.c.o:
	${CC} -c $< ${CFLAGS}

