CC = gcc
OBJS = chessSystem.o Game.o Players.o Tournament.o  utilities.o chessSystemTestsExample.o
EXEC = chess
DEBUG_FLAG = -g
COMP_FLAG = -std=c99 -Wall -pedantic-errors -Werror

$(EXEC) : $(OBJS)
	$(CC) $(COMP_FLAG) $(DEBUG_FLAG) $(OBJS) -o $@ -L. -lmap
chessSystem.o : chessSystem.c chessSystem.h map.h Players.h Game.h Tournament.h utilities.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
Game.o : Game.c Game.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
chessSystemTestsExample.o : tests/chessSystemTestsExample.c chessSystem.h  test_utilities.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) tests/chessSystemTestsExample.c
Players.o : Players.c Players.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
Tournament.o : Tournament.c Tournament.h map.h Game.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
utilities.o : utilities.c utilities.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS) $(EXEC)