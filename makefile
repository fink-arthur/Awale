CC = clang
CFLAGS= -c -ansi -pedantic -std=c++0x -Wall -Ofast
MOVE = /bin/mv -f
RM = /bin/rm -f
BIN = ./bin

EXE=bin/exec
LIBS=
LDFLAGS= -lm -lstdc++

SOURCES = $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp)
ENTETES = $(wildcard src/*.h src/*/*.h src/*/*/*.h)

OBJS = ${SOURCES:.cpp=.o}


ifdef DEBUG
	CFLAGS += -g
endif

all: veryclean obj exec run

obj: ${SOURCES}
	${CC} ${CFLAGS} ${SOURCES} -o ${OBJS}



exec: ${OBJS}
	${CC} ${OBJS} -o ${EXE} ${LDFLAGS} ${LIBS}


run:
	cd bin;./exec

.PHONY: clean veryclean

clean : #pas de dependance
	/bin/rm -f *.o *.gch *.bak *~ src/*.o src/*/*.o src/*/*/*.o

veryclean : clean
	/bin/rm -f ${EXE}
