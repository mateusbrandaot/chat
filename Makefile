TARGET = client

SOURCE = src
BIN = bin
INC = headers

SRC = $(wildcard $(SOURCE)/*.c)
OBJ = $(patsubst %,$(BIN)/%, $(notdir $(SRC:.c=.o)))

CC = gcc
FLAGS = -Wall -I$(INC)

$(BIN)/$(TARGET):	$(OBJ)
	$(CC) -pthread -o $@ $^

$(BIN)/%.o:	$(SOURCE)/%.c
	$(CC) $(FLAGS) -c $< -o $@

run:	
	./$(BIN)/$(TARGET)