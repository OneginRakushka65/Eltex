CC      := gcc
AR      := ar
CFLAGS  := -Wall -Wextra -std=c11 -O2
PROJECT := Eltex_6.1
APP_DIR := Eltex_6.1
LIB_DIR := PhonebookLib
LIB_BASENAME := PhonebookLib

ifeq ($(OS),Windows_NT)
    EXE   := .exe
else
    EXE   :=
endif

RMRF  = rm -rf "$(1)"
MKDIR = mkdir -p "$(1)"

BUILD_DIR := build
BIN_DIR   := bin
STATIC_LIB := $(BIN_DIR)/lib$(LIB_BASENAME).a
TARGET    := $(BIN_DIR)/$(PROJECT)$(EXE)

LIB_SRC := $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ := $(patsubst $(LIB_DIR)/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SRC))

APP_SRC := $(wildcard $(APP_DIR)/*.c)
APP_OBJ := $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/app/%.o,$(APP_SRC))

DEPS := $(LIB_OBJ:.o=.d) $(APP_OBJ:.o=.d)

.PHONY: all lib clean rebuild run help

all: $(TARGET)
lib: $(STATIC_LIB)

$(STATIC_LIB): $(LIB_OBJ) | $(BIN_DIR)
	$(AR) rcs $@ $^

$(TARGET): $(APP_OBJ) $(STATIC_LIB) | $(BIN_DIR)
	$(CC) -o $@ $(APP_OBJ) -L$(BIN_DIR) -l$(LIB_BASENAME)

$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.c | $(BUILD_DIR)/lib
	$(CC) $(CFLAGS) -I$(LIB_DIR) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/app/%.o: $(APP_DIR)/%.c | $(BUILD_DIR)/app
	$(CC) $(CFLAGS) -I$(APP_DIR) -I$(LIB_DIR) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/lib:
	$(call MKDIR,$@)

$(BUILD_DIR)/app:
	$(call MKDIR,$@)

$(BIN_DIR):
	$(call MKDIR,$@)

clean:
	$(call RMRF,$(BUILD_DIR))
	$(call RMRF,$(BIN_DIR))

rebuild: clean all

run: all
	./$(TARGET)

help:
	@echo "Targets:"
	@echo "  all     - build static lib + application"
	@echo "  lib     - build only the static library"
	@echo "  run     - build and run"
	@echo "  clean   - remove build artifacts"
	@echo "  rebuild - clean + all"

-include $(DEPS)