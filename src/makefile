.PHONY: clean
CC=gcc
RM=rm -rf
CFLAGS=-Wall
TARGET=sdl

ifeq (${OPTC}X,X)
	CFLAGS+=-O2
else
	CFLAGS+=${OPTC}
endif

SRCS=${wildcard *.c}
OBJS=${SRCS:.c=.o}
LDFLAGS =-Wall
LDFLAGS+=-lSDL2
LDFLAGS+=-lSDL2_ttf

all:${TARGET}
	@echo "*** Done ***"
${TARGET}:${OBJS}
	$(file >$@.in) $(foreach O,$^,$(file >>$@.in,$O))
	${CC} @$@.in ${LDFLAGS} -o $@
	@rm $@.in
clean:
	${RM} ${TARGET} *~ *.o

.c.o:
	${CC} ${CFLAGS} -c $<
