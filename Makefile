OBJS = Agent.c ArrayList.c main.c Game.c Matrix.c
CC = gcc
INCLUDE_PATHS = -IC:\TDM-GCC-64\include
LIBRARY_PATHS = -IC:\TDM-GCC-64\lib
COMPILER_FLAGS = -w -Wl,-subsystem,windows
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2
OBJ_NAME = SharkFatal
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)