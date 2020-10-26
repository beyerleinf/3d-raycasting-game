# OBJS specifies which files to compile as part of the project
OBJS = src/*.cpp src/utils/*.cpp src/engine/*.cpp

# CC specifies which compiler we're using
CC = g++

ifeq ($(OS), Windows_NT)
	# Windows specifics go here
	INCLUDE_PATHS = -IC:\dev\mingw-libs\include -Iinclude
	LIBRARY_PATHS = -LC:\dev\mingw-libs\lib
	# COMPILER_FLAGS = -w -Wl,-subsystem,windows
	COMPILER_FLAGS = -std=c++1
	LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2_image  -lSDL2
else
	# Linux specifics go here
	INCLUDE_PATHS = -Iinclude
	COMPILER_FLAGS = -std=c++11
	LINKER_FLAGS = -lSDL2_image -lSDL2main -lSDL2
endif

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = build/game

# This is the target that compiles our executable
all : $(OBJS)
	mkdir -p build
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# This is for cleaning the build folder
clean:
	rm -rf build