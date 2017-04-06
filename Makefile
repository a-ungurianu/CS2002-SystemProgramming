CC = clang
LDFLAGS = -g
CFLAGS = -std=c99 -Wall -Wextra -g

SRC_DIR = src
OBJ_DIR = obj

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c
	-mkdir -p ${OBJ_DIR}
	${CC} ${CFLAGS} -c $^ -o $@


runcmds : ${OBJ_DIR}/runcmds.o
	${CC} ${LDFLAGS} $^ -o $@

shellparse : ${OBJ_DIR}/shellparse.o
	${CC} ${LDFLAGS} $^ -o $@

clean :
	-rm -r ${OBJ_DIR}
	-rm runcmds shellparse
