# Determina il sistema operativo
ifeq ($(OS), Windows_NT)
    OS_TYPE := Windows
else
    OS_TYPE := $(shell uname)
endif

# Configurazione del compilatore
CC = gcc
CFLAGS = -std=c99 -Wshadow -Wconversion -Wfloat-equal -g
SRC_DIR = src
OBJ_DIR = build
SAVES_DIR = saves

# Opzioni specifiche per Linux
ifeq ($(OS_TYPE), Linux)
    CFLAGS += -fsanitize=address,undefined
    BIN = unstable_students
else
# Opzioni specifiche per Windows
    BIN = unstable_students.exe
# Imposta la codifica UTF-8 (estesa) per la console
	CHCP = chcp 65001 >nul
endif

# Trova i file sorgente e oggetto
SRC = $(wildcard $(SRC_DIR)/*/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Regole principali
all: $(BIN)

$(BIN): $(OBJ)
ifeq ($(OS_TYPE), Windows)
	@$(CHCP)
	@if not exist "$(SAVES_DIR)" mkdir "$(SAVES_DIR)"
else
	mkdir -p $(SAVES_DIR)
endif
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
ifeq ($(OS_TYPE), Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	mkdir -p $(dir $@)
endif
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS_TYPE), Windows)
	@if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)"
	@if exist "$(BIN)" del /Q /F "$(BIN)"
else
	rm -rf $(OBJ_DIR) $(BIN)
endif

run: all
ifeq ($(OS_TYPE), Windows)
	$(BIN) $(ARGS)
else
	./$(BIN)
endif

gdb: all
	gdb $(BIN)

valgrind: all
ifeq ($(OS_TYPE), Linux)
	valgrind --leak-check=full --tool=memcheck -s $(BIN)
else
	@echo L'opzione valgrind è disponibile solo su Linux.
endif
