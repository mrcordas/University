CC = gcc
CFLAGS = -c -Wall -pedantic -Wextra
#defina os diretórios abaixo sempre com o / exe: src/
SRC_DIR = src/
OBJ_DIR = obj/
DEBUG_DIR = debug/

PROG_NAME = app.exe

ifdef SRC_DIR
SRC = $(wildcard $(SRC_DIR)*.c)
else
SRC = $(wildcard *.c)
endif

OBJ = $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(SRC:.c=.o))

.PHONY: all
all: checks clearScreen $(PROG_NAME) run

$(PROG_NAME) : $(OBJ)
	@$(CC) $(OBJ) -o $(DEBUG_DIR)$(PROG_NAME)
	@echo "Compilação terminada!" 

$(OBJ_DIR)%.o : $(SRC_DIR)%.c
	@$(CC) $(CFLAGS) $< -o $@

#$< pre-requisito
#$@ alvo

#.PHONY previne alvos sem pre-requisitos com nome igual a arquivo no diretório

.PHONY: rebuild
rebuild: clean all

.PHONY: clean
clean:
	@rm -rf $(OBJ) $(DEBUG_DIR)*

.PHONY: run
run:
	@echo running....
	@echo
	@cd $(DEBUG_DIR) && ./$(PROG_NAME)
#	@./$(DEBUG_DIR)$(PROG_NAME)
	
.PHONY:clearScreen
clearScreen:
	@clear

.PHONY: checks
checks:
ifdef OBJ_DIR
	@mkdir -p $(OBJ_DIR)
endif
ifdef DEBUG_DIR
	@mkdir -p $(DEBUG_DIR)
endif