
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

SRC_C = $(wildcard $(SRC_DIR)/*.c)
OBJ_C = $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(notdir $(SRC_C))))	

server : $(OBJ_C)
	@if [ ! -d bin ]; then mkdir bin; fi
	@gcc -o bin/server $^ -lpthread

$(OBJ_DIR)/%.o : %.c
	@if [ ! -d obj ]; then mkdir obj; fi
	@gcc -c $< -o $@

clean :
	rm bin/*
	rm obj/*