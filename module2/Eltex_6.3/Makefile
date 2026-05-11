CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2
PROJECT := Eltex_6.3
APP_DIR := Eltex_6.3

ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE :=
endif

RMRF  = rm -rf "$(1)"
MKDIR = mkdir -p "$(1)"

BUILD_DIR  := build
BIN_DIR    := bin
PLUGIN_DIR := $(BIN_DIR)/plugins
TARGET     := $(BIN_DIR)/$(PROJECT)$(EXE)

PLUGINS := \
    $(PLUGIN_DIR)/Sum.dll  \
    $(PLUGIN_DIR)/Diff.dll \
    $(PLUGIN_DIR)/Mult.dll \
    $(PLUGIN_DIR)/Div.dll

APP_SRC := $(wildcard $(APP_DIR)/*.c)
APP_OBJ := $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/app/%.o,$(APP_SRC))
DEPS    := $(APP_OBJ:.o=.d)

.PHONY: all clean rebuild run help

all: $(TARGET) $(PLUGINS)

$(PLUGIN_DIR)/Sum.dll:  SumLib/sum.c   | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $<

$(PLUGIN_DIR)/Diff.dll: DiffLib/diff.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $<

$(PLUGIN_DIR)/Mult.dll: MultLib/mult.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $<

$(PLUGIN_DIR)/Div.dll:  DivLib/div.c   | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $<

$(TARGET): $(APP_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^

$(BUILD_DIR)/app/%.o: $(APP_DIR)/%.c | $(BUILD_DIR)/app
	$(CC) $(CFLAGS) -I$(APP_DIR) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/app:
	$(call MKDIR,$@)

$(BIN_DIR):
	$(call MKDIR,$@)

$(PLUGIN_DIR): | $(BIN_DIR)
	$(call MKDIR,$@)

clean:
	$(call RMRF,$(BUILD_DIR))
	$(call RMRF,$(BIN_DIR))

rebuild: clean all

run: all
	cd $(BIN_DIR) && ./$(PROJECT)$(EXE)

help:
	@echo "Targets:"
	@echo "  all     - build application + all plugins"
	@echo "  plugins - build only the plugins"
	@echo "  run     - build and run"
	@echo "  clean   - remove build artifacts"
	@echo "  rebuild - clean + all"

-include $(DEPS)