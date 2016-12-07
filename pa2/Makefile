#PA2 makefile
#adding @ will not show on the monitor
# $@: represent target
# $<: represent first dependency
CC = gcc
CFLAGS = -O3
TARGET = constraint_LEA
SRC = main.cpp router.cpp
OBJS = $(SRC:.cpp=.o)

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o $@
	echo "compilation cuccess..."
%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $@


.PHONY:clean:
clean:
	@rm -f
