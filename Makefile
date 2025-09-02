CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src
SRC_DIR = src
OBJ_DIR = obj
BIN = unstable_students

SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: clean
