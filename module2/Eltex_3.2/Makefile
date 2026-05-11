CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
PROJECT := Eltex_3.2
SRC_DIR := Eltex_3.2

ifeq ($(OS),Windows_NT)
    EXE   := .exe
else
    EXE   :=
endif

RMRF  = rm -rf "$(1)"
MKDIR = mkdir -p "$(1)"

BUILD_DIR := build
BIN_DIR   := bin
TARGET    := $(BIN_DIR)/$(PROJECT)$(EXE)

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

.PHONY: all clean rebuild run

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -MMD -MP -c -o $@ $<

$(BUILD_DIR):
	$(call MKDIR,$@)

$(BIN_DIR):
	$(call MKDIR,$@)

clean:
	$(call RMRF,$(BUILD_DIR))
	$(call RMRF,$(BIN_DIR))

rebuild: clean all

run: all
	./$(TARGET)

-include $(DEPS)