CC = g++
FLAGS = -std=c++11 -O0 -Wall -Wno-unused-function 

BUILD_FOLDER = bin
SRC_FOLDER = src
LIBS_FOLDER = lib
INCLUDES_FOLDER = include

PROGRAM_NAME = MineGL
TARGETS = main.cpp $(SRC_FOLDER)/game.cpp $(SRC_FOLDER)/glad.c $(SRC_FOLDER)/textrendering.cpp
LIBS = libglfw3.a
LIBS_FLAGS = -lrt -lm -lXrandr -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor

BIN_PATH = $(BUILD_FOLDER)/$(PROGRAM_NAME)

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(TARGET)
	$(CC) $(FLAGS) -I $(INCLUDES_FOLDER) -o $(BIN_PATH) $(TARGETS) $(LIBS_FOLDER)/$(LIBS) $(LIBS_FLAGS)
	./$(BIN_PATH)

clean:
	$(RM) $(BIN_PATH)
