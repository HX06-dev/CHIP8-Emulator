# CHIP-8 Emulator Makefile

CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -Iinclude
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

TARGET := $(BIN_DIR)/chip8

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run debug release

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: all
	./$(TARGET)

debug: CFLAGS += -g -O0
debug: clean all

release: CFLAGS += -O2 -DNDEBUG
release: clean all

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)