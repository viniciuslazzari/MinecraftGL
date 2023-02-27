CC = g++
FLAGS = -std=c++17 -O0 -Wall -Wextra -Wpedantic -Wno-unused-function 

BUILD_FOLDER = bin
SRC_FOLDER = src
LIBS_FOLDER = lib
INCLUDES_FOLDER = include

PROGRAM_NAME = MineGL
TARGETS = main.cpp $(wildcard $(SRC_FOLDER)/*.cpp) $(wildcard $(SRC_FOLDER)/lib/*.c) 
LIBS = libglfw3.a
LIBS_FLAGS = -lrt -lm -lXrandr -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor

BIN_PATH = $(BUILD_FOLDER)/$(PROGRAM_NAME)

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(TARGET)
	mkdir -p $(BUILD_FOLDER)
	$(CC) $(FLAGS) -I $(INCLUDES_FOLDER) -o $(BIN_PATH) $(TARGETS) $(LIBS_FOLDER)/$(LIBS) $(LIBS_FLAGS)
	./$(BIN_PATH)

clean:
	$(RM) $(BIN_PATH)
