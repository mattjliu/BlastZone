SRC_DIR = src
OBJ_DIR = obj
IN_DIR = include
RAYLIB_DIR = C:\raylib

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CC = $(RAYLIB_DIR)\mingw\bin\g++.exe

CXXFLAGS = $(RAYLIB_DIR)\raylib\src\raylib.rc.data -fopenmp -Wall -g -I$(IN_DIR)

LDFLAGS = -fopenmp -lmsvcrt -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32 -lshell32 -luser32 -Wl,--subsystem,console

main: $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS) && $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CXXFLAGS)
