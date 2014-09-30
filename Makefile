INC_DIR	= ./include
SRC_DIR = ./src
OBJ_DIR	= ./object

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_PATH  = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJ_FILES))

LIBS	= 
CC	= /usr/bin/g++
CFLAGS	= -I$(INC_DIR)

all: assignment1

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -g -c -o $@ $< $(CFLAGS)

assignment1: $(OBJ_PATH)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ assignment1
