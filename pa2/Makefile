#PA2 makefile
#adding @ will not show on the monitor
# $@: represent target
# $<: represent first dependency

CC = g++
CFLAGS = -O3
TARGET = constraint_LEA
SRC = main.cpp Router.cpp
OBJS = $(SRC:.cpp=.o)


${TARGET}: ${OBJS}
	@${CC} ${CFLAGS} ${OBJS} -o $@
	@echo "compilation success..."
%.o: %.cpp Router.h
	@${CC} ${CFLAGS} -c $< -o $@


.PHONY:clean
clean:
	@rm -f ${OBJS} ${TARGET}
