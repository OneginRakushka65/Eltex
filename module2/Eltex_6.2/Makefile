CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2
PROJECT := Eltex_6.2
APP_DIR := Eltex_6.2
LIB_DIR := PhonebookLib
LIB_BASENAME := PhonebookLib

ifeq ($(OS),Windows_NT)
    EXE        := .exe
    SHLIB      := $(LIB_BASENAME).dll
    IMPORT_LIB := lib$(LIB_BASENAME).dll.a
else
    EXE        :=
    SHLIB      := lib$(LIB_BASENAME).so
    IMPORT_LIB := $(SHLIB)
    PIC        := -fPIC
endif

RMRF  = rm -rf "$(1)"
MKDIR = mkdir -p "$(1)"

BUILD_DIR  := build
BIN_DIR    := bin
SHARED_LIB := $(BIN_DIR)/$(SHLIB)
TARGET     := $(BIN_DIR)/$(PROJECT)$(EXE)

LIB_SRC := $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ := $(patsubst $(LIB_DIR)/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SRC))

APP_SRC := $(wildcard $(APP_DIR)/*.c)
APP_OBJ := $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/app/%.o,$(APP_SRC))

DEPS := $(LIB_OBJ:.o=.d) $(APP_OBJ:.o=.d)

.PHONY: all lib clean rebuild run help

all: $(TARGET)
lib: $(SHARED_LIB)

$(SHARED_LIB): $(LIB_OBJ) | $(BIN_DIR)
	$(CC) -shared -Wl,--out-implib,$(BIN_DIR)/$(IMPORT_LIB) -o $@ $^

$(TARGET): $(APP_OBJ) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) -o $@ $(APP_OBJ) -L$(BIN_DIR) -l$(LIB_BASENAME)

$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.c | $(BUILD_DIR)/lib
	$(CC) $(CFLAGS) $(PIC) -DPHONEBOOKLIB_EXPORTS -I$(LIB_DIR) -MMD -MP -c -o $@ $<

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
	cd $(BIN_DIR) && ./$(PROJECT)$(EXE)

help:
	@echo "Targets:"
	@echo "  all     - build dynamic lib + application"
	@echo "  lib     - build only the dynamic library"
	@echo "  run     - build and run"
	@echo "  clean   - remove build artifacts"
	@echo "  rebuild - clean + all"

-include $(DEPS)